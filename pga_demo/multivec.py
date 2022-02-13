from clifford import Cl, conformalize
from math import pi, e, sin, cos
import plotly.graph_objects as go
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
FPS = 60

G3, blades_g3 = Cl(3)
G3c, blades_g3c, stuff = conformalize(G3)
locals().update(blades_g3c)
locals().update(stuff)

fig, ax = plt.subplots()

def project(gaze_vec, vec):
    return (vec^gaze_vec/(vec|gaze_vec))*gaze_vec

def make_frame(points):
    points2dict = lambda points: dict(zip(('x','y'),zip(*points)))
    edges = [go.Scatter(**points2dict([(points_proj[i][e1], points_proj[i][e2]) for i in line])) for line in lines]
        #code for animation obtained from https://towardsdatascience.com/basic-animation-with-matplotlib-and-plotly-5eef4ad6c5aa
    button = {
        "type": "buttons",
        "buttons": [
            {
                "label": "Play",
                "method": "animate",
                "args": [None, {"frame": {"duration": 20}}],
            }
        ],
    }    
    layout = go.Layout(updatemenus=[button],
                       title_text=f"",yaxis_range=[-1,1], xaxis_range=[-1,1])  
    frame = go.Frame(
        data=edges,
        layout=go.Layout(title_text=f"",yaxis_range=[-1,1], xaxis_range=[-1,1])
    )
    return frame, layout, edges

def plot_3d(points, lines):
    plt.clf()
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    for line in lines:
        x,y,z = list(zip(*[(points[i][e1], points[i][e2], points[i][e3]) for i in line]))
        ax.plot(x,z,y)

points = [2*(i&1)*e1+2*((i>>1)&1)*e2+2*((i>>2)&1)*e3 for i in range(8)]
lines = [(0,1),(0,2),(0,4),(1,3),(1,5),(2,3),(2,6),(3,7),(4,5),(4,6),(5,7),(6,7)]
cube_origin = - e1 - e2 + 1*e3
points = [p  + cube_origin for p in points]
conformal_points = list(map(up, points))
gaze_vec= e3

T_0 = 1+einf*cube_origin/2
T_1 = einf*(-1*e1+1*e2+5*e3)/2
T_2 = einf*(2*e1-2*e2)/2
R = ((e1+e2+e3)*e123/abs(e1+e2+e3))*(1/3)*pi

duration = 2
num_frames = duration*FPS

rotors=[T_0*e**(T_1+(T_2)*(i/(duration*FPS)))*e**((R)*(i/(duration*FPS)))*~T_0 for i in range(duration*FPS+1) ]
layout = go.Layout(title_text="Animation")
gaze_vecs = [cos(pi/6*(i/num_frames))*e3+sin(pi/6*(i/num_frames))*e1 for i in range(duration*FPS+1)]
edges = []
frames = []
for i in range(duration*FPS+1):
    points_proj = [project(gaze_vecs[i], down(rotors[i]*p*~rotors[i])) for p in conformal_points]
    frame, layout, edges = make_frame(points_proj)
    frames.append(frame)
fig = go.Figure(data=edges, frames=frames, layout=layout)
fig.show()
fig.write_html("rotate_across_changing_fov.html", auto_play=False)
