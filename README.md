# fast_glb_mesh_loader

fast_glb_mesh_loader is a high-performance Python extension for extracting vertices and faces from .glb files. It doesn't bother loading animations, textures or anything other than the base mesh from a .glb file and so is much faster than other .glb loaders. 

## Features
- Rapid extraction of vertices and faces from .glb files.
- Integration with numpy for convenient data handling and manipulation.
- Seamless integration with Python programs.

## Installation

### Via pip

You can install `fast_glb_mesh_loader` directly from pip:

```bash
pip install fast_glb_mesh_loader
```

### From Source

1. Clone the repository:

```bash
git clone https://github.com/samconsidine/fast_glb_mesh_loader.git
```

2. Navigate to the cloned directory and run the following:

```bash
python setup.py install
```

## Usage

To use `fast_glb_mesh_loader` in your Python program:

```python
import fast_glb_mesh_loader

result = fast_glb_mesh_loader.verts_and_faces_from_file("path_to_your_glb_file.glb")
vertices = result["vertices"]
faces = result["faces"]
```

Where:
- `path_to_your_glb_file.glb` is the path to your .glb file.
- `vertices` will be a numpy array containing the vertices.
- `faces` will be a numpy array containing the faces.

## Dependencies

- [numpy](https://numpy.org/): for data representation and manipulation.

## License

MIT License

### Third party licenses

- TinyGLTF : Licensed under the MIT License http://opensource.org/licenses/MIT. Copyright (c) 2017 Syoyo Fujita, Aurélien Chatelain and many contributors.
- json.hpp : Licensed under the MIT License http://opensource.org/licenses/MIT. Copyright (c) 2013-2017 Niels Lohmann http://nlohmann.me.
- stb_image : Public domain.
- catch : Copyright (c) 2012 Two Blue Cubes Ltd. All rights reserved. Distributed under the Boost Software License, Version 1.0.
- RapidJSON : Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip. All rights reserved. http://rapidjson.org/
- dlib(uridecode, uriencode) : Copyright (C) 2003 Davis E. King Boost Software License 1.0. http://dlib.net/dlib/server/server_http.cpp.html

## Contributing

Contributions are welcome!

## Issues

If you encounter any problems or have suggestions, please open an issue.