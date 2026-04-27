1. Add light
  - Read normal vectors from .obj file
  - Normalize the normal vectors
  - use a dot product with a light vector
2. Triangulate non-triangular faces
  - Convex only for now, concavity at a much later date
3. Do not reject all triangles with any point behind the camera, instead split the triangle into more triangles such that we instead render the portion of the triangle that is visible
4. Add the scaling factor for the verticies in the OBJ
