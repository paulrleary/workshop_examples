import SchemDraw as schem
import SchemDraw.elements as e


d = schem.Drawing()


right = {'cnt':3, 'labels':['GPIO0', 'GPIO1', 'GPIO2']}
top = {'cnt':1, 'labels':['Vcc']}
bot = {'cnt':1, 'labels':['GND']}

Ard = e.blackbox(d.unit*1.5, d.unit*2.25, 
                   rinputs=right, tinputs=top, binputs=bot,
                   leadlen=1, mainlabel='UNO')
                        

A = d.add(Ard)
d.add(e.DOT_OPEN, label = '$+5V$', xy = A.Vcc)
GND = d.add(e.GND, xy = A.GND)
d.add(e.LINE, d = 'right', xy = A.GPIO0)
L = d.add(e.RBOX, d= 'down', label = '$Load$')

##S = d.add(e.SWITCH_SPST,d= 'down', label = '$in$')


##N = d.add(e.DOT)
##d.add(e.LINE, d = 'right', xy = R1.end, l = d.unit*0.25)
##d.add(e.DOT_OPEN, label = '$V_2$')
##R2 = d.add(e.RES, xy = R1.end,d= 'down', label = '$R_{2}$')


##I = d.add(e.ARROW_I, d= 'down',)
GND = d.add(e.GND)
##d.loopI([Vin,R,GND], 'I', pad=1.25)
##d.draw()
d.draw(showplot=False)
d.save('ARDPWR.eps')
