1. Add light
  - Read normal vectors from .obj file
  - Normalize the normal vectors
  - use a dot product with a light vector
2. Triangulate non-triangular faces
  - Convex only for now, concavity at a much later date
3. Do not reject all triangles with any point behind the camera, instead split the triangle into more triangles such that we instead render the portion of the triangle that is visible
4. Lineraly interpolate between normal vectors instead of just always using the normal vector for A
5. As it stands, a face should still be lit if the light vector is facing in the same direction as a normal, it should only be lit if we are rendering the front of the face (remember signedTriangleArea is negative if the points are  counter-clockwise) and the light vector points in the opposite direction of the normal, or if we are rendering the backface, if the light vector points in the same direction as the normal
6. Optomize the recursion in getAbsolutePosition and getBasisVectorsRecursive (also instead of recalculating getBasisVectorsRecursive, we should accumulate it in getAbsolutePosition)
