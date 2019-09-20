from mayavi import mlab
from numpy import pi, cos, sin, mgrid


class MFigs(object):
    def __init__(self, name='', xdata=0, ydata=0, zdata=0):
        self.fhandler = mlab.figure(name)
        mlab.mesh(xdata, ydata, zdata, figure=self.fhandler)
        self.azymuth = (mlab.view())[0]
        self.delazy = 0
        self.zenith = (mlab.view())[1]
        self.delzen = 0


_CONST_NGROUPS = 2


# DATA INITIALIZATION
#"""A very pretty picture of spherical harmonics translated from
#the octaviz example."""
dphi, dtheta = pi / 250.0, pi / 250.0
[phi, theta] = mgrid[0:pi + dphi * 1.5:dphi, 0:2 * pi + dtheta * 1.5:dtheta]
m0 = 4; m1 = 3; m2 = 2; m3 = 3; m4 = 6; m5 = 2; m6 = 6; m7 = 4;
r = sin(m0*phi)**m1 + cos(m2*phi)**m3 + sin(m4*theta)**m5 + cos(m6*theta)**m7
x = r * sin(phi) * cos(theta)
y = r * cos(phi)
z = r * sin(phi) * sin(theta)

mm = [MFigs(name='Grupo Alpha', xdata=x, ydata=y, zdata=z), MFigs(name='Grupo Beta', xdata=x, ydata=y, zdata=z)]


# Ppal periodic function, for graphics
@mlab.animate()
def figureUARTmanipulation(mfigs):
    while ser.is_open:

        if ser.inWaiting() > 3:
            input_str = ser.readline().decode()
            n = len(input_str) - 1
            index = int(input_str[0])-1
            if index > _CONST_NGROUPS:
                print('Group outnumbered!')
            else:
                fig = mfigs[index]
                angle = float(input_str[2:n])

                if input_str[1] == 'R':
                    # if (fig.delazy != angle)
                    fig.delazy = angle
                    fig.azymuth += angle
                if input_str[1] == 'C':
                    # if (fig.delazen != angle)
                    fig.delzen = angle
                    fig.zenith += angle

                mlab.view(azimuth=fig.azymuth, elevation=fig.zenith, figure=fig.fhandler)
                fig.fhandler.render()
        yield

import serial
ser = serial.Serial('COM5', baudrate=1200, parity=serial.PARITY_ODD)  # open serial port
#ser.write_timeout(5)
figureuartmanipulation(mfigs=mm)
mlab.show()
ser.close()
# if ser.is_open:
    # ser.write(logo.logo_png)
    #ser.write(b'The brown quick fox jumps over the lazy dog.\n')
    #ser.write(logo.Lorem_Ipsum)
# while ser.is_open:
    # if ser.inWaiting():
        # input_str = ser.read_until().decode()
        # print(input_str, end=" ")


