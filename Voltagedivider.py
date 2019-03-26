import SchemDraw as schem
import SchemDraw.elements as e
d = schem.Drawing()
Vin = d.add(e.DOT_OPEN , label='$V_1$')
R1 = d.add(e.RES, d= 'down', label = '$R_1$')
N = d.add(e.DOT)
d.add(e.LINE, d = 'right', xy = R1.end, l = d.unit*0.25)
d.add(e.DOT_OPEN, label = '$V_2$')
R2 = d.add(e.RES, xy = R1.end,d= 'down', label = '$R_{2}$')
GND = d.add(e.GND)
d.draw(showplot=False)
d.save('Voltagedivider.eps')
