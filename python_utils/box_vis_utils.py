import numpy as np

def xywh_to_corners(data):
    """
    Takes a coordinate xywh as input
    Return tlbr as output
    Note that input must be of size (1,4) to use
    """
    data = np.array(data)
    data = data.astype(float)
    data[:,0] = data[:,0] - data[:,2]/2
    data[:,1] = data[:,1] - data[:,3]/2
    data[:,2] = data[:,0] + data[:,2]
    data[:,3] = data[:,1] + data[:,3]

    return data.astype(int)