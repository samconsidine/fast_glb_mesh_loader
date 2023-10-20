import matplotlib.pyplot as plt
import fast_glb_mesh_loader

data = fast_glb_mesh_loader.extract_vertices_and_faces_from_file('cats_head_from_the_cutty_sark.glb')[0]

x, y, z = data[:, 0], data[:, 1], data[:, 2]

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

ax.scatter(x, y, z)

plt.show()
