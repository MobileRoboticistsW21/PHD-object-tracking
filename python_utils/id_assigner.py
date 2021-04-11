import numpy as np
from sklearn.metrics import pairwise_distances

def assign_ids_postfact(data):
    output_0 = np.concatenate([np.array(data[0]['bb']), np.array(data[0]['flows'])], axis=1)

    # arbitary idx for frame 0
    id_0 = np.arange(output_0.shape[0]) + 1
    data[0]['ids'] = id_0.tolist()


    # loop over rest of frames
    for i_frame in range(1, len(data)):
        output_1 = np.concatenate([np.array(data[i_frame]['bb']), np.array(data[i_frame]['flows'])], axis=1)

        # compute pairwise distance between targets in previous and current iteration
        dist = pairwise_distances(output_0, output_1) 

        id_1 = -1 * np.ones(output_1.shape[0])

        for i in range(min(output_0.shape[0], output_1.shape[0])): 
            # find smallest value in dist
            min_idx = np.argmin(dist)

            # calcualte row and col of that value
            min_col_idx = min_idx % output_1.shape[0]
            min_row_idx = min_idx // output_1.shape[0]

            # keep the id for next iter
            id_1[min_col_idx] = id_0[min_row_idx]

            # fill row and col with infinity so same previous target would not be re-used, 
            # and would not try to find id for same current target
            dist[:, min_col_idx].fill(np.inf)
            dist[min_row_idx, :].fill(np.inf)

        id_1 = id_1.astype(int)

        # more targets in current than last iter, assign new ids 
        if (output_0.shape[0] < output_1.shape[0]):
            # which ones in id_1 need to be filled with new id
            idx_to_fix = np.where(id_1 == -1)[0]

            new_id = 1

            for i in range(output_1.shape[0] - output_0.shape[0]):
                while new_id in id_1:
                    new_id += 1

                id_1[idx_to_fix[i]] = new_id
            

        data[i_frame]['ids'] = id_1.tolist()

        output_0 = output_1
        id_0 = id_1
    
    return data