#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include "tiny_gltf.h"
#include <Python.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>


tinygltf::Model load_gltf(char* fpath) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, fpath);

    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
    }

    return model;
}
static PyObject* verts_and_faces_from_file(PyObject* self, PyObject* args) {
    char* fpath;
    
    if (!PyArg_ParseTuple(args, "s", &fpath)) {
        return NULL;
    }

    tinygltf::Model model = load_gltf(fpath);

    if (model.meshes.empty()) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to load glTF file");
        return NULL;
    }

    std::vector<float> positions;
    std::vector<int> indices;

    // perf: alloc before moving data
    size_t totalVertices = 0;
    size_t totalIndices = 0;
    for (const tinygltf::Mesh& mesh : model.meshes) {
        for (const tinygltf::Primitive& primitive : mesh.primitives) {
            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                totalVertices += model.accessors[primitive.attributes.at("POSITION")].count;
            }
            if (primitive.indices != -1) {
                totalIndices += model.accessors[primitive.indices].count;
            }
        }
    }

    positions.reserve(3 * totalVertices);
    indices.reserve(3 * totalIndices);

    for (const tinygltf::Mesh& mesh : model.meshes) {
        for (const tinygltf::Primitive& primitive : mesh.primitives) {
            // vertices
            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("POSITION")];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
                int byteStride = bufferView.byteStride ? bufferView.byteStride : 3 * sizeof(float);

                if (byteStride == 3 * sizeof(float) && accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    const float* srcData = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset]);
                    positions.insert(positions.end(), srcData, srcData + 3 * accessor.count);
                } else {  // TODO: figure out if this is even necessary
                    for (size_t i = 0; i < accessor.count; i++) {
                        float x = *reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + byteStride * i]);
                        float y = *reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + byteStride * i + sizeof(float)]);
                        float z = *reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + byteStride * i + 2 * sizeof(float)]);

                        positions.push_back(x);
                        positions.push_back(y);
                        positions.push_back(z);
                    }
                }
            }

            // indices
            bool warnUnexpectedIndexFormat = false;
            if (primitive.indices != -1) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                for (size_t i = 0; i < accessor.count; i++) {
                    int idx;
                    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                        idx = static_cast<int>(buffer.data[bufferView.byteOffset + i]);
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                        idx = *reinterpret_cast<const unsigned short*>(&buffer.data[bufferView.byteOffset + i * sizeof(unsigned short)]);
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                        idx = *reinterpret_cast<const unsigned int*>(&buffer.data[bufferView.byteOffset + i * sizeof(unsigned int)]);
                    } else {
                        // unsupported index format, just carry on as if nothing happened
                        if (not warnUnexpectedIndexFormat) {
                            warnUnexpectedIndexFormat = true;
                            printf("Warning: unexpected index format %d\n", accessor.componentType);
                        }
                        continue;
                    }
                    indices.push_back(idx);
                }
            }
        }
    }

    npy_intp dims_positions[2] = {static_cast<npy_intp>(positions.size() / 3), 3};
    PyObject* py_array_positions = PyArray_SimpleNew(2, dims_positions, NPY_FLOAT);
    memcpy(PyArray_DATA((PyArrayObject*)py_array_positions), positions.data(), positions.size() * sizeof(float));

    npy_intp dims_indices[2] = {static_cast<npy_intp>(indices.size() / 3), 3}; 
    PyObject* py_array_indices = PyArray_SimpleNew(2, dims_indices, NPY_INT);
    memcpy(PyArray_DATA((PyArrayObject*)py_array_indices), indices.data(), indices.size() * sizeof(int));

    // Create a new Python dictionary
    PyObject* result_dict = PyDict_New();
    
    // Set the items for the keys "vertices" and "faces"
    PyDict_SetItemString(result_dict, "vertices", py_array_positions);
    PyDict_SetItemString(result_dict, "faces", py_array_indices);

    // Decrement the reference count for the numpy arrays since the dictionary holds a reference to them now
    Py_DECREF(py_array_positions);
    Py_DECREF(py_array_indices);

    return result_dict;
}



static PyMethodDef methods[] = {
    {"verts_and_faces_from_file", verts_and_faces_from_file, METH_VARARGS, "Get a numpy array from the vertices of the glb asset file"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "fast_glb_mesh_loader",
    NULL,
    -1,
    methods
};

PyMODINIT_FUNC PyInit_fast_glb_mesh_loader(void) {
    import_array();
    return PyModule_Create(&module);
}