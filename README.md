## TODO

- [ ] Simulation playback
    - [ ] record simulation
    - [ ] playback
- [ ] Fix TODOs
- [ ] Better (and more efficient) voxel drawing
    - [ ] custom shaders? only pass color + alpha to the GPU
- [ ] Decrease compilation time (pimpl)
- [ ] Formatter
- [ ] Profiling - when simulation is done
- [ ] CPython interface?
- [ ] [draw architecture](https://plantuml.com/component-diagram)
    - [ ] refactor ?

## Performance

For 3D GoL 50^3 cells. All times in ms. Simulating until the whole space is filled with cubes. In fullscreen

| Commit  | Render 50^3 | Simulation 50^3 | Render 100^3 | Simulation 100^3 |
| ---     | ---         | ---             | ---          | ---              |
| b54ec9c | 40.161      | 3.97671         | 312.757      | 381.396          | 
