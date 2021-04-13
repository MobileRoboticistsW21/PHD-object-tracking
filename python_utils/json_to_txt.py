import json
import numpy as np

# Function converted bbox center coordinates to top left coordinates
def bbox_convert(c_w_h):
    c_x, c_y = c_w_h[0], c_w_h[1]
    w , h = c_w_h[2], c_w_h[3]
    tl_x = c_x - w / 2
    tl_y = c_y - h / 2
    return [tl_x, tl_y, w, h]

def json_to_txt(json_file_name):
    # Read data from json file, length is the total frames number
    f = open(json_file_name +'.json')
    data = json.load(f)
    frames_num = len(data)

    # Initialize numpy array to store data from json file
    data_in_np = -1 * np.ones([1,9])

    # Append each line of data
    for i in range(frames_num):
        obj_num = len(data[i]['ids'])
        for j in range(obj_num):
                bb = bbox_convert(data[i]['bb'][j])
                data_in_np = np.vstack((data_in_np,[int(i+1), data[i]['ids'][j], bb[0], bb[1], bb[2], bb[3], int(1), int(1), int(1)]))

    # Delete first row that is for initialization
    data_in_np = np.delete(data_in_np, (0), axis=0)

    # Sort arrayy so that it is in the same format as the evaluation kit
    sorted_array = data_in_np[np.lexsort((data_in_np[:,0], data_in_np[:,1]))]
    np.savetxt(json_file_name +'.txt', sorted_array, delimiter = ',', fmt='%d')
