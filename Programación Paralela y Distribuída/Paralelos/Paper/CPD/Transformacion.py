'''
!play girls and boys blur

main = crystal castles
creditos = shootin star or gone jr jr

'''
import tkinter as tk
#import PIL
#import cv2
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import winsound
import math

########################################################################
########################################################################
toplot = 0
pi2 = 6.28318530718
delta_theta = pi2 / 50
########################################################################
########################################################################
def T(equis,yes):
    #(-y,2x-y)#
    x=[]
    y=[]
    for i in range(len(equis)):
        x.append(yes[i]*(-1))
        y.append((2*equis[i]) - yes[i])

    return x,y
def S(equis,yes):
    #(-x,-y)#
    x=[]
    y=[]
    for i in range(len(equis)):
        x.append(equis[i]*(-1))
        y.append(yes[i]*(-1))

    return x,y

def plotOriginal():
    global toplot
    global delta_theta
    global pi2
    fig = plt.figure()
    ax = fig.add_subplot(111)
    if(toplot == 3):
        x=[0,1,1,0]
        y=[0,0,1,0]
        lines = plt.Line2D(x,y)
        ax.add_line(lines)
    elif(toplot == 1):
        cx = 1
        cy = 0
        r = 1
        theta = 0.0
        x = [(cx + (r* math.sin(theta)))]
        y = [(cy + (r* math.cos(theta)))]
        while(theta <= pi2):
            theta += delta_theta
            xx = (cx + (r* math.sin(theta)))
            yy = (cy + (r* math.cos(theta)))
            x.append(xx)
            y.append(yy)
        #print(len(x))
        lines = plt.Line2D(x,y)

    ax.add_line(lines)    
    ax.set_xlim(min(x)-1,max(x)+1)
    ax.set_ylim(min(y)-1,max(y)+1)
    plt.title('Figura Original')
    lly=plt.Line2D([0,0],[10,-10])
    llx=plt.Line2D([-10,10],[0,0])
    ax.add_line(lly)
    ax.add_line(llx)
    ax.get_lines()[0].set_color("red")
    ax.get_lines()[1].set_color("black")
    ax.get_lines()[2].set_color("blue")

    
    plt.show()
        
def plotT():
    x,y=T([1,0],[0,1])
    print('|',x[0],' ',x[1],'|')
    print('|',y[0],' ',y[1],'|')
    global toplot
    global delta_theta
    global pi2
    fig = plt.figure()
    ax = fig.add_subplot(111)
    if(toplot == 3):
        x,y = T([0,1,1,0],[0,0,1,0])
        lines = plt.Line2D(x,y)
    elif(toplot == 1):
        cx = 1
        cy = 0
        r = 1
        theta = 0.0
        x = [(cx + (r* math.sin(theta)))]
        y = [(cy + (r* math.cos(theta)))]
        while(theta <= pi2):
            theta += delta_theta
            xx = (cx + (r* math.sin(theta)))
            yy = (cy + (r* math.cos(theta)))
            x.append(xx)
            y.append(yy)
        x,y = T(x,y)
        lines = plt.Line2D(x,y)
    ax.add_line(lines)
    ax.set_xlim(min(x)-1,max(x)+1)
    ax.set_ylim(min(y)-1,max(y)+1)
    plt.title('T(x,y) = (-y , 2x-y)')
    lly=plt.Line2D([0,0],[10,-10])
    llx=plt.Line2D([-10,10],[0,0])
    ax.add_line(lly)
    ax.add_line(llx)
    ax.get_lines()[0].set_color("red")
    ax.get_lines()[1].set_color("black")
    ax.get_lines()[2].set_color("blue")

    plt.show()

    
    
def plotS():
    x,y=T([1,0],[0,1])
    x,y=S(x,y)
    xx,yy=S([1,0],[0,1])
    print('|',x[0],' ',x[1],'| > |',xx[0],' ',xx[1],'|')
    print('|',y[0],' ',y[1],'| > |',yy[0],' ',yy[1],'|')
    global toplot
    global delta_theta
    global pi2
    fig = plt.figure()
    ax = fig.add_subplot(111)
    if(toplot == 3):
        x,y = T([0,1,1,0],[0,0,1,0])
        x,y = S(x,y)
        lines = plt.Line2D(x,y)
    elif(toplot == 1):
        cx = 1
        cy = 0
        r = 1
        theta = 0.0
        x = [(cx + (r* math.sin(theta)))]
        y = [(cy + (r* math.cos(theta)))]
        while(theta <= pi2):
            theta += delta_theta
            xx = (cx + (r* math.sin(theta)))
            yy = (cy + (r* math.cos(theta)))
            x.append(xx)
            y.append(yy)
        x,y = T(x,y)
        x,y = S(x,y)
        lines = plt.Line2D(x,y)
    ax.add_line(lines)
    ax.set_xlim(min(x)-1,max(x)+1)
    ax.set_ylim(min(y)-1,max(y)+1)
    plt.title('T(x,y) = (-x , -y)')
    lly=plt.Line2D([0,0],[10,-10])
    llx=plt.Line2D([-10,10],[0,0])
    ax.add_line(lly)
    ax.add_line(llx)
    ax.get_lines()[0].set_color("red")
    ax.get_lines()[1].set_color("black")
    ax.get_lines()[2].set_color("blue")

    plt.show()
    

        
def dibujarTriangulos(ist):
    global toplot
    toplot = ist

def dibujarcirculos(ist):
    global toplot
    toplot = ist
########################################################################
########################################################################
def create_demo():
    global toplot
    window = tk.Toplevel(root)
    window.title("Demo")
    window.geometry("500x250")
    bt= tk.Button(window, text="Triangulo",command =lambda: dibujarTriangulos(3))
    bc= tk.Button(window, text="Circulo",command =lambda: dibujarcirculos(1))
    bt.place(x=40,y=170,width=75,height=30)
    bc.place(x=157,y=170,width=75,height=30)
    tri = tk.Label(window,
                   text="punto 1 = ( 0 ; 0 )\npunto 2 = ( 1 ; 0 )\npunto 3 = ( 1 ; 1 )",
                   font=("Helvetica",10))
    cir = tk.Label(window,
                   text="centro = ( 0 ; 0 )\nradio   =   1   ",
                   font=("Helvetica",10))
    tri.place(x=30,y=90)
    cir.place(x=145,y=90)
    titt = tk.Label(window,text="EJEMPLO DE CLASE",font=("courier",20))
    titt.place(x=10,y=10)
    bO = tk.Button(window,text = "Original", font=("courier",17),command =lambda: plotOriginal())
    bT = tk.Button(window,text = "T()", font=("courier",17),command =lambda: plotT())
    bS = tk.Button(window,text = "S()", font=("courier",17),command =lambda: plotS())
    bO.place(x=320,y=75,width=130,height=40)
    bT.place(x=320,y=130,width=130,height=40)
    bS.place(x=320,y=190,width=130,height=40)

########################################################################
########################################################################
def create_find():
    window = tk.Toplevel(root)
    window.title("Encontrar Transformacion")
    window.geometry("600x600")

########################################################################
########################################################################
to_x=[]
to_y=[]
import parser
def addto(my_x,my_y):
    global to_x
    global to_y
    to_x.append(int(my_x))
    to_y.append(int(my_y))
    print(to_x[len(to_x)-1])
    print(to_y[len(to_y)-1])

def F(equis,yes):
    x=[]
    y=[]
    for i in range(len(equis)):
        x.append((equis[i]*2)-(yes[i])) #2x-y
        y.append((yes[i])+(equis[i]))   #x+y

    ### ( 2x-y , x+y )
    return x,y
    
def drawT(the_x,the_y):
    global to_x
    global to_y
    mpl.style.use('default')
    x,y=F([1,0],[0,1])
    print('|',x[0],' ',x[1],'|')
    print('|',y[0],' ',y[1],'|')
    fig = plt.figure()
    ax = fig.add_subplot(111)
    x,y = F(to_x,to_y)
    lines = plt.Line2D(x,y)
    lones = plt.Line2D(to_x,to_y)
    ax.add_line(lines)
    ax.add_line(lones)
    ax.set_xlim(min(x+to_x)-1,max(x+to_x)+1)
    ax.set_ylim(min(y+to_y)-1,max(y+to_y)+1)
    plt.title('Transformacion')
    ax.get_lines()[0].set_color("black")
    lly=plt.Line2D([0,0],[10,-10])
    llx=plt.Line2D([-10,10],[0,0])
    ax.add_line(lly)
    ax.add_line(llx)
    ax.get_lines()[1].set_color("black")
    ax.get_lines()[2].set_color("blue")

    plt.show()
    
    
    
########################################################################
########################################################################
def create_ingr():
    window = tk.Toplevel(root)
    window.title("Dada una Transformacion")
    window.geometry("330x300")
    tit = tk.Label(window,text="Transformacion\nPersonalizada",font=("courier",15))
    tit.place(x=55,y=10)
    compX = tk.Entry(window,bd=4)
    compY = tk.Entry(window,bd=4)
    compX.place(x=20,y=80, width=55)
    compY.place(x=100,y=80, width=55)
    
    addd = tk.Button(window,text="Añadir",font=("courier",18),
                     command=lambda:addto(compX.get(),compY.get()))
    addd.place(x=20,y = 125, width=130,height = 30)

    tranX = tk.Entry(window,bd=4)
    tranY = tk.Entry(window,bd=4)
    tranX.place(x=60,y=240, width=85)
    tranY.place(x=170,y=240, width=85)

    draw = tk.Button(window,text="Calcular\ny\nDibujar",font=("courier",16),
                     command=lambda:drawT(tranX.get(),tranY.get()))
    draw.place(x=185,y=70, width=130,height = 130)
    

########################################################################
########################################################################
def create_cred():
    window = tk.Toplevel(root)
#    root.withdraw()
    window.title(" ")
    window.geometry("250x400")
    window.configure(background='gray')
########################################################################
########################################################################
root = tk.Tk()
root.title("Transformaciones")
root.geometry("400x350")
titu = tk.Label(root, text="TRANSFORMACIONES \n LINEALES", font=("courier",25))
titu.pack()
bD = tk.Button(root, text="Demo\nEjemplo", command=create_demo)
bH = tk.Button(root, text="Hallar\nTransformacion", command=create_find)
bR = tk.Button(root, text="Ingresar\nTrasformacion", command=create_ingr)
bD.place(x=25,y=100,width=110,height=180)
bH.place(x=140,y=100,width=110,height=180)
bR.place(x=260,y=100,width=110,height=180)
bC = tk.Button(root,text="__________", command=create_cred)
bC.place(x=60,y=300,width=280,height=25)

root.mainloop()
