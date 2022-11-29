#!/usr/bin/python3 -u
import asyncio
from asyncio.subprocess import PIPE
import argparse
import time

parser = argparse.ArgumentParser(description="ctbtMeteo - bsp and balloon atmosphere probing software with GUI")
parser.add_argument("-d", "--duration", default="120", help="pairable duration in seconds")
args = parser.parse_args()


init_strings = [
    b'power on\r',
    b'discoverable on\r',
    b'pairable on\r',
    b'agent off\r',
    b'agent KeyboardDisplay\r',
    b'default-agent\r'
]


class BT:
    def __init__(self):
        self.p = None
        self.ready = False
        self.init_step = -1
        self.init_done = False
        self.no_data_cnt = 0
        self.end_time = 0

    async def send(self, s):
        self.p.stdin.write(s)
        await self.p.stdin.drain()

    async def read(self):
        try:
            l = await asyncio.wait_for(self.p.stdout.readline(), 1)
        except asyncio.TimeoutError:
            pass
        else:
            if l is None:
                raise Exception('EOF')
            return l
        return None

    async def handle(self, l):
        if l is None:
            if self.init_step == -1:
                self.ready = True
                self.init_step = 0
            elif self.init_step < len(init_strings) and self.no_data_cnt > 2:
                self.ready = True
            else:
                return

        if l is not None and (
                'Agent unregistered' in l or 'succeeded' in l or 'Agent registered' in l or 'agent request successful' in l):
            self.init_step += 1
            self.ready = True

        if self.ready and self.init_step < len(init_strings):
            await self.send(init_strings[self.init_step])
            self.ready = False
            return

        if 'Authorize service\n' in l:
            print("Asking for service authorization...")
            await self.send(b'yes\r')
            print("Yes sent")
            return

        if 'Request confirmation' in l:
            print("Asking for pair confirmation...")
            await self.send(b'yes\r')
            print("Yes sent")
            return

    async def run(self):
        self.end_time = time.time() + int(args.duration)
        self.p = await asyncio.create_subprocess_shell('bluetoothctl', stdin=PIPE, stdout=PIPE)

        while self.end_time > time.time():
            try:
                l = await self.read()
            except:
                break
            else:
                if l is None:
                    self.no_data_cnt += 1
                    await self.handle(None)
                else:
                    self.no_data_cnt = 0
                    await self.handle(str(l))
                continue

        print("Times up! Exiting!")
        await self.send(b'pairable off\r')
        await self.send(b'exit\r')

        return await self.p.wait()


bt = BT()
loop = asyncio.get_event_loop()
loop.run_until_complete(bt.run())
