## TODO

- [x] add SDL to VS solution - [guide here](https://github.com/libsdl-org/SDL/blob/main/docs/INTRO-visualstudio.md)
- [x] migrate to SDL3
- [ ] Finish math tests
- [x] Fix camera rotate
- [x] Fix camera pan
- [x] Fix 2D GoL rules
- [x] Fix timing
- [x] Add pause
- [x] 3D GoL
- [x] Performance measurement
- [ ] GoL performance improvement
- [ ] -Wall
- [ ] Fix TODOs
- [ ] Better (and more efficient) voxel drawing
    - [x] Wireframe mode
    - [x] Alpha - change visibility?
    - [ ] custom shaders? only pass color + alpha to the GPU
- [ ] Fluid simulation
- [ ] Decrease compilation time (pimpl)
- [ ] CPython interface?
- [ ] [draw architecture](https://plantuml.com/component-diagram)

## Performance

For 3D GoL 50^3 cells. All times in ms. Simulating until the whole space is filled with cubes. In fullscreen

| Commit  | Render 50^3 | Simulation 50^3 | Render 100^3 | Simulation 100^3 |
| ---     | ---         | ---             | ---          | ---              |
| b54ec9c | 40.161      | 3.97671         | 312.757      | 381.396          | 
