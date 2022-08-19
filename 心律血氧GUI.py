from tkinter import *
from tkinter import ttk

#引入easygui模組
import easygui as gui

#串列阜
import serial
from serial.tools import list_ports as sport
ser = serial.Serial('COM5', 115200, timeout=1)

#建立視窗
root = Tk()
root.geometry('1100x700')
root.title('Pulse & SpO2')
root.resizable(False, False)
root.config(bg='#475f94')

#載入圖檔素材
from PIL import ImageTk, Image, ImageSequence
im12 = ImageTk.PhotoImage(Image.open('image12.png'))
im13 = ImageTk.PhotoImage(Image.open('image13.png'))
im14 = ImageTk.PhotoImage(Image.open('image14.gif'))
im15 = ImageTk.PhotoImage(Image.open('image15.gif'))

#建立畫布
ca = Canvas(root, width=500, height= 450)
ca.place(x=300, y= 160)
ca.config(bg='#000000')
ca.create_line(0, 225, 500, 225, fill='#77FF00') #繪製中線

#建立標籤風格
label_style=ttk.Style(root)
label_style.configure("My.TLabel", padding=2, relief='groove', background='white', foreground='#008080', font=('Arial Black', 13))

#建立port標籤
port_label = ttk.Label(root, text = 'Port', anchor='center', font=('Arial Black', 12), background = '#ffda03', foreground = '#1b2431')
port_label.place(x=20, y=20, width = 50, height = 30)

#建立File標籤
lb = ttk.Label(root, text='File',anchor='center', font=('Arial Black', 12), background = '#ffda03', foreground = '#1b2431')
lb.place(x=400,y= 20 ,width = 50, height = 30)

#建立time標籤
port_label = ttk.Label(root, text = 'time(s)', anchor='center', font=('Arial Black', 12), background = '#475f94', foreground = 'yellow')
port_label.place(x=500, y=620, width = 70, height = 30)

#建立數據標籤(心率 & 血氧)
lb_nn = Label(root, text='Data', fg='yellow', bg='black')
lb_nn.place(x=310, y=165, width=35, height=30)
lb_nn.config(textvariable ='nn',font=('Arial Black', 10))

lb_hh = Label(root, text='Data', fg='yellow', bg='black')
lb_hh.place(x=310, y=190, width=35, height=30)
lb_hh.config(textvariable ='hh',font=('Arial Black', 10))


lb_nn1 = Label(root, text='bpm', fg='yellow', bg='black',font=('Arial Black', 10))
lb_nn1.place(x=360, y=165, width=40, height=30)

lb_hh1 = Label(root, text='SpO2(%)', fg='yellow', bg='black',font=('Arial Black', 10))
lb_hh1.place(x=355, y=190, width=80, height=30)

#文字輸入方塊
ent = ttk.Entry(root, width=40, textvariable='ent')
ent.place(x=460, y=20, height=30)

#建立進度棒
pg = ttk.Progressbar(root)
pg.place(x=850, y = 160, height = 450)
pg.config(value=0, maximum=2600, variable='pg', orient='vertical', mode ="determinate")
root.setvar('pg', 2000)

#建立按鈕風格
btn_style = ttk.Style()
btn_style.configure('TButton', font=('Arial Black', 10), foreground = 'black', background = 'gray', width = 12, relief = RAISED)

#建立下拉式選單
cb = ttk.Combobox(root, height=5, width=6, textvariable='cb')
cb.place(x=80,y=20,height=30)

#填入COMBOBOX內容
sary = []
for c in list(sport.comports()): #[['COM3', 'USB Serial Port (COM3)', 'USB VID:PID=0403:6001 SER=AB0LUT4CA']]裡頭長這樣，如果有2個以上comport則會繼續增加
    sary.append(c[0])  #取出c，['COM3', 'USB Serial Port (COM3)', 'USB VID:PID=0403:6001 SER=AB0LUT4CA']，c[0]為'COM3'
    print(c)
cb.config(values = sary, font=('Arial Black', 10))
root.setvar('cb', sary[0]) 


#繪製數據
line = None
line1 = None
br = []
br1 =[]
def draw(ar, ar1):
    global br, br1, line , line1
    cr = []
    cr1= []
    br += ar
    br1 += ar1
    br = br[-500:]
    br1 = br1[-500:]
    for x, y in enumerate(br):
        cr += [(x,(y//4))]
    for x, y in enumerate(br1):
        cr1 += [(x,(y//4))]
    ca.delete(line)
    ca.delete(line1)
    line = ca.create_line(cr, width = 2, fill = '#77FF00')
    line1 = ca.create_line(cr1, width = 2, fill = '#FF69B4')

#讀取comport資料
def read_port():
    global conn, ser, fp, spo2
    ar=[]
    ar1=[]
    while conn:
        for i in range(10):
            s = ser.readline()
            a, b, c, d= s.split(b' ')
            print(a, b, c, d)
            try:
                a = int(a.decode())
                b = int(b.decode())
                c = int(c.decode())
                d = round(float(d.decode()))
                a -= 1420000
                a /= 10
                b -= 1700000
                b /= 10
                ar += [a]
                ar1 += [b]
                d=d/100
                root.setvar('hh', d)
                #print(d)
                SPO2['text'] = str(d) + ' %'
                
                #-----bpm & spo2-----
                if c > 0 :
                    root.setvar('nn', c)
                    root.setvar('pg', c)
                    root.setvar('hh', d)
                    Heart_rate['text'] = str(c) + ' bpm'
                    SPO2['text'] = str(d) + ' %'
                    if fp:
                        fp.write("{} bpm , {} SpO2(%)\n".format(int(c), int(d)))
            except:
                pass
            if conn is False:
                return
        draw(ar, ar1)
    
#定義連線/斷線按鈕與事件
port = root.getvar('cb')

def ser_conn(e):
    global conn, ser
    if btn_conn['text'] == 'Connect':
        conn = True
        btn_conn['text'] = 'Disconnect'
        if ser.isOpen():
            ser.close()
        try:
            port = root.getvar('cb')
            ser = serial.Serial(port, 115200, timeout=1)
            ser.flush()
            th_conn()
        except:
            gui.msgbox('無法開啟 Comoport', '連線失敗')
    else:
        conn = False
        btn_conn['text'] = 'Connect'
        ser.close()

#定義瀏覽按鈕與事件
def proc_brow(e):
    global fp
    tx = btn_brow.cget('text')
    if tx == 'Browse...':
        fn = gui.filesavebox('熱敏電阻', '紀錄', '*.txt')
        if fn is None:
            return
        root.setvar('ent', fn)
        btn_brow.config(text='Record')
    elif tx == 'Record':
        fn = root.getvar('ent')
        fp = open (fn, 'w')
        btn_brow.config(text='Stop')
    else:
        fp.close()
        fp = None
        btn_brow.config(text='Browse...')
    
#建立連線/斷線按鈕
btn_conn = ttk.Button(root, text='Connect', style='TButton')
btn_conn.place(x=165, y=20, height=30)
btn_conn.bind('<ButtonRelease-1>', ser_conn)

#建立數據紀錄按鈕
btn_brow = ttk.Button(root, text='Browse...', style='TButton')
btn_brow.place(x=755,y=20,height=30)
btn_brow.bind('<ButtonRelease-1>', proc_brow)

#Threading
import threading
def th_conn():
    t3 = threading.Thread(target=read_port)
    t3.start()

#建立血氧量測介面
#Oxygen = Label(root, image = im13, cursor = 'hand2', bg='#475f94') 
#Oxygen.place(x = 90, y = 380)

f1 = ttk.LabelFrame( root, text = 'SpO₂', width = 150 , height = 80)
f1.place(x = 90, y = 530)

SPO2 = ttk.Label(f1, text = '   %', anchor = 'center', style = 'My.TLabel')
SPO2.place(x = 20, y = 10, height = 40, width = 105)

#建立心率量測介面
#Heart = Label(root, image = im14, cursor = 'hand2',bg='#475f94') 
#Heart.place(x = 100, y = 100)

f0 = ttk.LabelFrame(root, text='Heart rate', width=150 , height=80)
f0.place(x = 90, y = 270)

Heart_rate=ttk.Label(f0, text ='bpm', anchor = 'center', style = 'My.TLabel')
Heart_rate.place(x = 20, y = 10, height = 40, width = 105)


#gif圖
from itertools import count

class ImageLabel(ttk.Label):
    def load(self, im):
        if isinstance(im, str):
            im = Image.open(im)
        self.loc = 0
        self.frames = []

        try:
            for i in count(1):
                self.frames.append(ImageTk.PhotoImage(im.copy()))
                im.seek(i)
        except EOFError:
            pass

        try:
            self.delay = im.info['duration']
        except:
            self.delay = 100

        if len(self.frames) == 1:
            self.config(image=self.frames[0])
        else:
            self.next_frame()

    def unload(self):
        self.config(image="")
        self.frames = None

    def next_frame(self):
        if self.frames:
            self.loc += 1
            self.loc %= len(self.frames)
            self.config(image=self.frames[self.loc])
            self.after(self.delay, self.next_frame)
            
lbl = ImageLabel(root)
lbl.load('image14.gif')
lbl.configure(background='#475f94')
lbl.place(x = 100, y = 150)

lb2 = ImageLabel(root)
lb2.load('image15.gif')
lb2.configure(background='#475f94')
lb2.place(x = 90, y = 380)

mainloop()
