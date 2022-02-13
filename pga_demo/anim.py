from sklearn.datasets import make_regression
X, y = make_regression(n_features = 1, noise=50, random_state=543)
def linear_regression(X, y, m_current=0, b_current=0, epochs=1000, learning_rate=0.0001):
    N = float(len(y))

    ms = [m_current] # a place to store our m values
    bs = [b_current] # a place to store our b values

    for i in range(epochs):
        y_current = (m_current * X) + b_current
        cost = sum([data**2 for data in (y-y_current)]) / N
        m_gradient = -(2/N) * sum(X * (y - y_current))
        b_gradient = -(2/N) * sum(y - y_current)
        m_current = m_current - (learning_rate * m_gradient)
        b_current = b_current - (learning_rate * b_gradient)

        ms.append(m_current)
        bs.append(b_current)

    return ms, bs
# run the linear regression and store the m and b values
ms, bs = linear_regression(X.ravel(), y, 
                           epochs=200, learning_rate=0.01)
# build the lines, mx + b for all ms X's and b's
y_lines = (ms * X + bs).T # transpose for easier indexing
import plotly.graph_objects as go# create the scatter plot
points = go.Scatter(x=X.flatten(), y=y, mode='markers')# create initial line
line = go.Scatter(x=X.flatten(), y=y_lines[0])# create a layout with out title (optional)
layout = go.Layout(title_text="Gradient Descent Animation")# combine the graph_objects into a figure
fig = go.Figure(data=[points, line])
                                 
# to see what we have so far
fig.show()
# create a list of frames
frames = []# create a frame for every line y
for i in range(len(y_lines)):    # update the line
    line = go.Scatter(x=X.flatten(), y=y_lines[i])    # create the button
    button = {
        "type": "buttons",
        "buttons": [
            {
                "label": "Play",
                "method": "animate",
                "args": [None, {"frame": {"duration": 20}}],
            }
        ],
    }    # add the button to the layout and update the
    # title to show the gradient descent step
    layout = go.Layout(updatemenus=[button],
                       title_text=f"Gradient Descent Step {i}")    # create a frame object
    frame = go.Frame(
        data=[points, line],
        layout=go.Layout(title_text=f"Gradient Descent Step {i}")
    )# add the frame object to the frames list
    frames.append(frame)
# combine the graph_objects into a figure
fig = go.Figure(data=[points, line],
                frames=frames,
                layout = layout)
                                 
# show our animation!
fig.show()
