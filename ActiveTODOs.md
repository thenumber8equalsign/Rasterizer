1. Triangulate non-triangular faces
  - Convex only for now, concavity at a much later date
2. Do not reject all triangles with any point behind the camera, instead split the triangle into more triangles such that we instead render the portion of the triangle that is visible
3. As it stands, a face should still be lit if the light vector is facing in the same direction as a normal, it should only be lit if we are rendering the front of the face (remember signedTriangleArea is negative if the points are  counter-clockwise) and the light vector points in the opposite direction of the normal, or if we are rendering the backface, if the light vector points in the same direction as the normal
4. Superspampling?
