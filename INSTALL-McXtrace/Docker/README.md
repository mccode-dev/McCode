# Running McStas and McXtrace 3.5.40 in Docker / podman etc.

The container definition is available at [here](https://github.com/willend/jupyter-remote-desktop-proxy/tree/mcstas-mcxtrace-3.5) and is further on dockerhub under docker.io/mccode/mcstas-mcxtrace

To run the container locally, use e.g.
```podman run -p 8888:8888 -p 5173:5173 docker.io/mccode/mcstas-mcxtrace```

