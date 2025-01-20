import matplotlib.pyplot as plt
import numpy as np
from boosted_trees import *

def generate_dataA(n_points = 1000):
    d1x = np.linspace(0, 1, n_points).reshape((-1, 1))
    d1y = np.random.random(n_points).reshape((-1, 1))

    X = np.vstack((np.hstack((d1x, d1y)), np.hstack((d1x + 0.9, d1y))))
    y = np.hstack((np.zeros(n_points), np.ones(n_points)))
    return X, y

def generate_dataB(n_points = 1000):
    d1x = np.linspace(0, 1, n_points).reshape((-1, 1))
    d1y = np.random.random(n_points).reshape((-1, 1))

    X = np.hstack((d1x, d1y))
    y = (d1x > d1y).astype(np.float32).squeeze()
    return X, y

# https://glowingpython.blogspot.com/2017/04/solving-two-spirals-problem-with-keras.html
def generate_dataC(n_points = 1000, noise = 0.05):
    n = np.sqrt(np.random.rand(n_points,1)) * 780 * (2*np.pi)/360
    d1x = -np.cos(n)*n + np.random.rand(n_points,1) * noise
    d1y = np.sin(n)*n + np.random.rand(n_points,1) * noise

    X = np.vstack((np.hstack((d1x,d1y)),np.hstack((-d1x,-d1y))))
    y = np.hstack((np.zeros(n_points),np.ones(n_points)))
    return X, y

def classify_data(X, y):
    bct = train(X, y)
    
    yPred = []
    for i in range(X.shape[0]):
        p = predict(bct, X[i, :])
        yPred.append(p)

    return np.array(yPred)

def plot_data(X, yTrue, yPred):
    fig = plt.figure() 

    ax = fig.add_subplot(1, 2, 1)
    ax.plot(X[yTrue==0, 0], X[yTrue==0, 1], '.', label='class A')
    ax.plot(X[yTrue==1, 0], X[yTrue==1, 1], '.', label='class B')
    ax.legend()
    ax.set_title('true data')

    ax = fig.add_subplot(1, 2, 2)
    ax.plot(X[yPred==0, 0], X[yPred==0, 1], '.', label='class A')
    ax.plot(X[yPred==1, 0], X[yPred==1, 1], '.', label='class B')
    ax.legend()
    ax.set_title('predicted data')

    plt.show()


if __name__ == "__main__":
    X, yTrue = generate_dataC()
    yPred = classify_data(X, yTrue)
    plot_data(X, yTrue, yPred)
