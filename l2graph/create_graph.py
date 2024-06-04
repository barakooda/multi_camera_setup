import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import os

# Get the directory of the current script
script_dir = os.path.dirname(os.path.abspath(__file__))

# Load the CSV files using relative paths
file_path_gt = os.path.join(script_dir, "..", "csv_files", "ball_pos_gt.csv")
file_path_reality = os.path.join(script_dir, "..", "csv_files", "ball_pos_real.csv")

# Read the CSV files into pandas dataframes with error handling
df_gt = pd.read_csv(file_path_gt, on_bad_lines='skip')
df_reality = pd.read_csv(file_path_reality, on_bad_lines='skip')

# Check for discrepancies in the number of columns
if df_gt.shape[1] != 3 or df_reality.shape[1] != 3:
    raise ValueError("One or both of the CSV files do not have exactly three columns.")

# Convert the dataframes to numpy arrays
array_gt = df_gt.to_numpy()
array_reality = df_reality.to_numpy()

# Compute the L2 norm (Euclidean distance) between corresponding vectors in the two arrays
l2_distances = np.linalg.norm(array_gt - array_reality, axis=1)

# Create the new array of 2D vectors where the first element is the L2 distance and the second element is the index
array_2d_vectors = np.column_stack((l2_distances, np.arange(len(l2_distances))))

# Optionally display the array (using standard pandas)
print(pd.DataFrame(array_2d_vectors, columns=["L2 Distance", "Index"]))

# Create the plot with y-axis limit set to 200 mm, zoomed in
plt.figure(figsize=(10, 6))
plt.plot(array_2d_vectors[:, 1], array_2d_vectors[:, 0] * 1000)  # Convert L2 distance to mm
plt.xlabel('Index')
plt.ylabel('L2 Distance (mm)')
plt.title('L2 Distance between Vectors from Two Files (Zoomed in to 200 mm)')
plt.ylim(0, 200)  # Set the y-axis limit to zoom in
plt.grid(True)

# Save the plot as an image file using a relative path
output_file_path = os.path.join(script_dir, "..", "l2graph", "l2_distance_plot.png")
plt.savefig(output_file_path)

plt.show()
