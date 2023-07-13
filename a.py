import asyncio # 非同期I/O(ソケット通信も)
import discord # DiscordAPI
import serial_asyncio # 非同期シリアル通信
import settings # 設定用
import time # 遅延用
import datetime # 時刻表示

# julius
class JuliusClient:
    def __init__(self, host='localhost', port=10500):
        self._reader, self._writer = None, None
        self.host = host
        self.port = port

    async def connect(self):
        self._reader, self._writer = await asyncio.open_connection(self.host, self.port)

    async def close(self):
        if self._writer:
            self._writer.close()
            await self._writer.wait_closed()

    async def read_message(self):

        global systemState
        state = 0 #フレーズの順番管理
        phrases = ["雲をつかむ", "赤い蝶", "エメラルドの海"] #フレーズ順番登録

        res = ''
        global word
        word = ''
        while True:
            # 音声認識の区切りである「改行+.」がくるまで待つ
            while (res.find('\n.') == -1):
                # Juliusから取得した値を格納していく
                data = await self._reader.read(1024)
                res += data.decode()

            for line in res.split('\n'):
                # Juliusから取得した値から認識文字列の行を探す
                index = line.find('WORD=')

                if systemState == 2:
                    # 認識文字列があったら...
                    if index != -1:
                        # 認識文字列部分だけを抜き取る
                        line = line[index + 6 : line.find('"', index + 6)]
                        # 文字列の開始記号以外を格納していく
                        if line != '[s]':
                            word = word + line
                            print('word：' + word)

                if systemState == 2 and word and word != '[/s]':
                    serial_reader.send_message(b'w')
                    if word == phrases[state]:
                        state += 1
                        if state == 1:
                            serial_reader.send_message(b's')
                        if state == 2:
                            serial_reader.send_message(b'l')
                        if state == 3:
                            serial_reader.send_message(b'o')
                            asyncio.create_task(send_dm(settings.SUB_OWNER_ID, "合言葉で開けたよ！"))
                            state = 0
                    else:
                        serial_reader.send_message(b'n')
                        state = 0
                        await asyncio.sleep(1.0)
                        serial_reader.send_message(b'f')
                word = ''
            res = ''

#時刻取得して文字列を返す
def whattime():
    now = datetime.datetime.now()
    nowp = now.strftime('%m/%d %H:%M:%S')
    return nowp

# クライアントの生成
client = discord.Client(intents=discord.Intents.all())

# システムの状態管理変数(待機 1, 実行中 2, 停止0)
systemState = 1

# システムの状態変更
async def changestate(state):

    global systemState

    if state == 1:
        systemState = 1
        serial_reader.send_message(b'1')
        await client.change_presence(status=discord.Status.online, activity=discord.Game(name="のんびり"))

    if state == 2:
        systemState = 2
        serial_reader.send_message(b'2')
        await client.change_presence(status=discord.Status.dnd, activity=discord.Game(name="お仕事"))

    if state == 0:
        systemState = 0
        serial_reader.send_message(b'0')
        await client.change_presence(status=discord.Status.idle, activity=discord.Game(name="お休み"))

    return systemState


# 非同期シリアル通信
class SerialReader(asyncio.Protocol):
    # シリアル通信開始時
    def connection_made(self, transport):
        self.transport = transport
        print("シリアル通信開始")
        time.sleep(2)
        asyncio.create_task(changestate(systemState))

    # シリアル受信時
    def data_received(self, data):
        global systemState
        if data == b'g':
            asyncio.create_task(send_dm(settings.SUB_OWNER_ID, "**玄関前**から呼び出されてるよ！"))
        if data == b't':
            self.send_message(b't')
            asyncio.create_task(self.sleep())
            asyncio.create_task(send_dm(settings.SUB_OWNER_ID, "**建物前**から呼び出されてるよ！\nあける？"))
        if data == b'o':
            serial_reader.send_message(b'c')
            asyncio.create_task(changestate(1))
        if data == b'c':
            serial_reader.send_message(b'c')
            asyncio.create_task(changestate(1))
        if data == b'1':
            asyncio.create_task(changestate(1))
        if data == b'2':
            asyncio.create_task(changestate(2))
        if data == b'0':
            asyncio.create_task(changestate(0))

    async def sleep(self):
        await asyncio.sleep(2) #スピーカーの音とか処理時間
        asyncio.create_task(changestate(2))

    # シリアル通信終了時
    def connection_lost(self, exc):
        print("シリアル通信終了")

    # バッファが埋まったとき
    def pause_writing(self):
        print("シリアル通信バッファが満杯")
    # バッファが空になったとき
    def resume_writing(self):
        print("シリアル通信バッファが空")

    # シリアル送信時
    def send_message(self, message):
        self.transport.write(message)



# DM送信
async def send_dm(user_id, message):
    user = await client.fetch_user(user_id)
    await user.send(f"{whattime()}\n{message}")


# discordと接続した時に呼ばれる
@client.event
async def on_ready():
    print(f"{client.user}がサーバにログインしました")
    loop = asyncio.get_running_loop()

    global serial_reader
    _, serial_reader = await serial_asyncio.create_serial_connection(loop, SerialReader, settings.SERIALPORT, baudrate=9600)

    julius_client = JuliusClient()
    await julius_client.connect()
    time.sleep(2)
    asyncio.create_task(julius_client.read_message()) #バックグラウンドで実行するためにcreate_task(awaitだと終了するまで待つ)

# メッセージを受信した時に呼ばれる
@client.event
async def on_message(message):
    # 自分のメッセージを無効
    if message.author == client.user:
        return

    # メッセージと応答
    if systemState == 0:
        if message.content.startswith("おはよ"):
            await changestate(1)
        else:
            await message.channel.send("...zzz")
    else:
        if message.content.startswith("あけて"):
            if systemState == 1:
                await message.channel.send("呼び出し時だけ開けられるよ\n無理やり開けたいときは「あけなさい」と言ってね")
            if systemState == 2:
                serial_reader.send_message(b'o')
                await message.channel.send("あけるよーん")

        elif message.content.startswith("あけなさい"):
            await changestate(2)
            serial_reader.send_message(b'p')
            await message.channel.send("あけるよーん")

        elif message.content.startswith("おわり"):
            if systemState == 1:
                await message.channel.send("何もしてないよ？")
            if systemState == 2:
                serial_reader.send_message(b'c')
                await changestate(1)
                await message.channel.send("つかれたー")

        elif message.content.startswith("おやすみ"):
            if systemState == 1:
                await changestate(0)

        elif message.content.startswith("たてもの"):
            serial_reader.send_message(b'6')
            await message.channel.send("建物呼び出しテスト")

        elif message.content.startswith("げんかん"):
            serial_reader.send_message(b'7')
            await message.channel.send("玄関呼び出しテスト")
        else:
            await message.channel.send("?")

# クライアントの実行
client.run(settings.BOT_TOKEN)

