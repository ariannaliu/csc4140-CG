#include "student_code.h"
#include "mutablePriorityQueue.h"

using namespace std;

namespace CGL
{

  /**
   * Evaluates one step of the de Casteljau's algorithm using the given points and
   * the scalar parameter t (class member).
   *
   * @param points A vector of points in 2D
   * @return A vector containing intermediate points or the final interpolated vector
   */
  std::vector<Vector2D> BezierCurve::evaluateStep(std::vector<Vector2D> const &points)
  { 
    // TODO Task 1.
    if (points.size() == 1) {
      return points;
    }

    vector<Vector2D> nextControlPoints;
    for (int i=0; i<points.size() - 1; i++){
      Vector2D newPoint = (1-t)*points[i] + (t) * points[i+1];
      nextControlPoints.push_back(newPoint);
    }
    return nextControlPoints;
  }

  /**
   * Evaluates one step of the de Casteljau's algorithm using the given points and
   * the scalar parameter t (function parameter).
   *
   * @param points    A vector of points in 3D
   * @param t         Scalar interpolation parameter
   * @return A vector containing intermediate points or the final interpolated vector
   */
  std::vector<Vector3D> BezierPatch::evaluateStep(std::vector<Vector3D> const &points, double t) const
  {
    // TODO Task 2.
    if (points.size() == 1) {
      return points;
    }

    vector<Vector3D> nextControlPoints;
    for (int i=0; i<points.size() - 1; i++){
      Vector3D newPoint = (1-t)*points[i] + (t) * points[i+1];
      nextControlPoints.push_back(newPoint);
    }
    return nextControlPoints;
  }

  /**
   * Fully evaluates de Casteljau's algorithm for a vector of points at scalar parameter t
   *
   * @param points    A vector of points in 3D
   * @param t         Scalar interpolation parameter
   * @return Final interpolated vector
   */
  Vector3D BezierPatch::evaluate1D(std::vector<Vector3D> const &points, double t) const
  {
    // TODO Task 2.
    if (points.size() == 1) {
      return points[0];
    }

    vector<Vector3D> nextControlPoints;
    for (int i=0; i<points.size() - 1; i++){
      Vector3D newPoint = (1-t)*points[i] + (t) * points[i+1];
      nextControlPoints.push_back(newPoint);
    }
    return evaluate1D(nextControlPoints,t);
  }

  /**
   * Evaluates the Bezier patch at parameter (u, v)
   *
   * @param u         Scalar interpolation parameter
   * @param v         Scalar interpolation parameter (along the other axis)
   * @return Final interpolated vector
   */
  Vector3D BezierPatch::evaluate(double u, double v) const 
  {  
    // TODO Task 2.
    // go trough the rows first
    vector<Vector3D> verticalControlPoints;
    for (int i=0; i<controlPoints.size(); i++){
      verticalControlPoints.push_back(evaluate1D(controlPoints[i],u));
    }

    Vector3D results = evaluate1D(verticalControlPoints, v);

    return results;
  }

  Vector3D Vertex::normal( void ) const
  {
    // TODO Task 3.
    // Returns an approximate unit normal at this vertex, computed by
    // taking the area-weighted average of the normals of neighboring
    // triangles, then normalizing.
    HalfedgeCIter target = this->halfedge();
    Vector3D target_p = target->vertex()->position;

    Vector3D total_sum = Vector3D(0,0,0);
    
    HalfedgeCIter this_edge = target;
    HalfedgeCIter this_twin = target->twin();
    HalfedgeCIter next_edge = target->twin()->next();

    Vector3D p1 = this_twin->vertex()->position;
    Vector3D p2 = next_edge->next()->vertex()->position;
    Vector3D edge1 = target_p - p1;
    Vector3D edge2 = p2 - target_p;

    total_sum += cross(edge1, edge2)/2;

    this_edge = next_edge;

    while (this_edge != target){
      this_twin = this_edge->twin();
      next_edge = this_twin->next();

      p1 = this_twin->vertex()->position;
      p2 = next_edge->next()->vertex()->position;
      edge1 = target_p - p1;
      edge2 = p2 - target_p;
      total_sum += cross(edge1, edge2)/2;
      this_edge = next_edge;
    }
    
    return total_sum.unit();
  }

  EdgeIter HalfedgeMesh::flipEdge( EdgeIter e0 )
  {
    // TODO Task 4.
    // This method should flip the given edge and return an iterator to the flipped edge.
    if (e0->isBoundary()) return e0;
    // find all the HalfedgeIter
    HalfedgeIter h0 = e0->halfedge();
    HalfedgeIter h0_t = h0->twin();
    HalfedgeIter h1 = h0->next();
    HalfedgeIter h1_t = h1->twin();
    HalfedgeIter h2 = h1->next();
    HalfedgeIter h2_t = h2->twin();
    HalfedgeIter h3 = h0_t->next();
    HalfedgeIter h3_t = h3->twin();
    HalfedgeIter h4 = h3->next();
    HalfedgeIter h4_t = h4->twin();

    // find all vertex
    VertexIter v0 = h0->vertex();
    VertexIter v1 = h1->vertex();
    VertexIter v2 = h2->vertex();
    VertexIter v3 = h4->vertex();

    // find all edges
    // e0 already exist
    EdgeIter e1 = h1->edge();
    EdgeIter e2 = h2->edge();
    EdgeIter e3 = h3->edge();
    EdgeIter e4 = h4->edge();

    // find all faces
    FaceIter f0 = h0->face();
    FaceIter f1 = h0_t->face();


    // after spliting

    // update all the halfedge
    // void setNeighbors( HalfedgeIter next,
    //                         HalfedgeIter twin,
    //                         VertexIter vertex,
    //                         EdgeIter edge,
    //                         FaceIter face )

    h0->setNeighbors(h1, h0_t, v3, e0, f0);
    h0_t->setNeighbors(h3, h0, v2, e0, f1);
    
    h1->setNeighbors(h2, h2_t, v2, e2, f0);
    h1_t->setNeighbors(h1_t->next(), h4, v2, e1, h1_t->face());

    h2->setNeighbors(h0, h3_t, v0, e3, f0);
    h2_t->setNeighbors(h2_t->next(), h1, v0, e2, h2_t->face());
    
    h3->setNeighbors(h4, h4_t, v3, e4, f1);
    h3_t->setNeighbors(h3_t->next(), h2, v3, e3, h3_t->face());

    h4->setNeighbors(h0_t, h1_t, v1, e1, f1);
    h4_t->setNeighbors(h4_t->next(), h3, v1, e4, h4_t->face());


    // h1_t->setNeighbors(h1_t->next(), h1, v2, e1, h1_t->face());
    // h2_t->setNeighbors(h2_t->next(), h2, v0, e2, h2_t->face());
    // h3_t->setNeighbors(h3_t->next(), h3, v3, e3, h3_t->face());
    // 


    // update all vertex
    v0->halfedge() = h2;
    v1->halfedge() = h4;
    v2->halfedge() = h0_t;
    v3->halfedge() = h0;

    // update all edges
    e0->halfedge() = h0;
    e1->halfedge() = h4;
    e2->halfedge() = h1;
    e3->halfedge() = h2;
    e4->halfedge() = h3;

    // update all vertex
    f0->halfedge() = h0;
    f1->halfedge() = h0_t;

    return e0;
  }

  VertexIter HalfedgeMesh::splitEdge( EdgeIter e0 )
  {
    // TODO Task 5.
    // This method should split the given edge and return an iterator to the newly inserted vertex.
    // The halfedge of this vertex should point along the edge that was split, rather than the new edges.
    if (e0->isBoundary()) return e0->halfedge()->vertex();

    // original graph
    // find all the HalfedgeIter
    HalfedgeIter h0 = e0->halfedge();
    HalfedgeIter h1 = h0->next();
    HalfedgeIter h2 = h1->next();
    HalfedgeIter h3 = h2->twin();
    HalfedgeIter h4 = h1->twin();
    HalfedgeIter h5 = h0->twin();
    HalfedgeIter h6 = h5->next();
    HalfedgeIter h7 = h6->next();
    HalfedgeIter h8 = h7->twin();
    HalfedgeIter h9 = h6->twin();

    // find all vertex
    VertexIter v0 = h0->vertex();
    VertexIter v1 = h1->vertex();
    VertexIter v2 = h2->vertex();
    VertexIter v3 = h7->vertex();

    // find all edges
    // e0 exist
    EdgeIter e1 = h1->edge();
    EdgeIter e2 = h2->edge();
    EdgeIter e3 = h6->edge();
    EdgeIter e4 = h7->edge();

    // find all faces
    FaceIter f0 = h0->face();
    FaceIter f1 = h5->face();

    // alloc new things
    // new halfedge

    HalfedgeIter h10 = newHalfedge();
    HalfedgeIter h11 = newHalfedge();
    HalfedgeIter h12 = newHalfedge();
    HalfedgeIter h13 = newHalfedge();
    HalfedgeIter h14 = newHalfedge();
    HalfedgeIter h15 = newHalfedge();


    // new vertices
    VertexIter v4 = newVertex();
    // v4->position = 0.5f * v3->position + 0.5f * v2->position;
    v4->isNew = true;
    v4->position = (v3->position + v2->position)/2;

    // new edges
    EdgeIter e5 = newEdge();
    EdgeIter e6 = newEdge();
    EdgeIter e7 = newEdge();
    e5->isNew = true;
    e6->isNew = true;
    e7->isNew = true;

    //new faces
    FaceIter f2 = newFace();
    FaceIter f3 = newFace();

    // split!!!!!!!!!!!

    // update all the halfedges
    // update all the halfedge
    // void setNeighbors( HalfedgeIter next,
    //                         HalfedgeIter twin,
    //                         VertexIter vertex,
    //                         EdgeIter edge,
    //                         FaceIter face )

    h0->setNeighbors(h10, h15, v0, e0, f0);
    h1->setNeighbors(h11, h4, v1, e1, f3);
    h2->setNeighbors(h0, h3, v2, e2, f0);
    h3->setNeighbors(h3->next(), h2, v0, e2, h3->face());
    h4->setNeighbors(h4->next(), h1, v2, e1, h4->face());
    h5->setNeighbors(h14, h12, v1, e7, f2);
    h6->setNeighbors(h13, h9, v0, e3, f1);
    h7->setNeighbors(h5, h8, v3, e4, f2);
    h8->setNeighbors(h8->next(), h7, v1, e4, h8->face());
    h9->setNeighbors(h9->next(), h6, v3, e3, h9->face());
    h10->setNeighbors(h2, h11, v4, e5, f0);
    h11->setNeighbors(h12, h10, v2, e5, f3);
    h12->setNeighbors(h1, h5, v4, e7, f3);
    h13->setNeighbors(h15, h14, v3, e6, f1);
    h14->setNeighbors(h7, h13, v4, e6, f2);
    h15->setNeighbors(h6, h0, v4, e0, f1);

    // update vertex
    v0->halfedge() = h0;
    v1->halfedge() = h5;
    v2->halfedge() = h11;
    v3->halfedge() = h13;
    v4->halfedge() = h15;

    // update edges
    e0->halfedge() = h15;
    e1->halfedge() = h1;
    e2->halfedge() = h2;
    e3->halfedge() = h6;
    e4->halfedge() = h7;
    e5->halfedge() = h10;
    e6->halfedge() = h13;
    e7->halfedge() = h5;

    // update faces

    f0->halfedge() = h2;
    f1->halfedge() = h6;
    f2->halfedge() = h7;
    f3->halfedge() = h1;
    
    return v4;
  }



  void MeshResampler::upsample( HalfedgeMesh& mesh )
  {
    // TODO Task 6.
    // This routine should increase the number of triangles in the mesh using Loop subdivision.
    // One possible solution is to break up the method as listed below.

    // 1. Compute new positions for all the vertices in the input mesh, using the Loop subdivision rule,
    // and store them in Vertex::newPosition. At this point, we also want to mark each vertex as being
    // a vertex of the original mesh.
    cout<<"!!!!!!!!!!!!!!!"<<endl;

    // iterate around all the vertex and find its new position
    for( VertexIter vertx = mesh.verticesBegin(); vertx != mesh.verticesEnd(); vertx++ )
    {
      // do something interesting with v
      vertx->isNew = false;
      int degree_count = 0;
      Vector3D original_neighbor_position_sum = Vector3D(0,0,0);
      HalfedgeIter h_start = vertx->halfedge();
      HalfedgeIter h = vertx->halfedge();
      do{
        HalfedgeIter h_twin = h->twin();
        VertexIter vertex_u = h_twin->vertex();
        original_neighbor_position_sum += vertex_u->position;
        degree_count++;
        h = h_twin->next();
      }while(h != h_start);
      

      float u;
      if (vertx->degree() == 3){
        u = 3.F/16.F;
      }else{
        u = 3.F/(8.F*vertx->degree());
      }
      vertx->newPosition =  (1 - vertx->degree() * u) * vertx->position + u * original_neighbor_position_sum;
      vertx->isNew = false;   
    }


    // 2. Compute the updated vertex positions associated with edges, and store it in Edge::newPosition.
    vector<EdgeIter> oldEdgeBuffer;
    for( EdgeIter old_edge = mesh.edgesBegin(); old_edge != mesh.edgesEnd(); old_edge++ )
    {
      
      // do something interesting with e
      HalfedgeIter old_edge_h = old_edge->halfedge();
      HalfedgeIter old_edge_h_t = old_edge_h->twin();
      VertexIter v_A = old_edge_h->vertex();
      VertexIter v_B = old_edge_h_t->vertex();
      VertexIter v_C = old_edge_h->next()->next()->vertex();
      VertexIter v_D = old_edge_h_t->next()->next()->vertex();
      old_edge->newPosition = 3.F/8.F * (v_A->position + v_B->position) + 1.F/8.F * (v_C->position + v_D->position);
      old_edge->isNew = false;
      oldEdgeBuffer.push_back(old_edge);
    }

    std::cout<<"Also Passed Here"<<endl;

    
    // 3. Split every edge in the mesh, in any order. For future reference, we're also going to store some
    // information about which subdivide edges come from splitting an edge in the original mesh, and which edges
    // are new, by setting the flat Edge::isNew. Note that in this loop, we only want to iterate over edges of
    // the original mesh---otherwise, we'll end up splitting edges that we just split (and the loop will never end!)
    

    for (int j=0; j<oldEdgeBuffer.size(); j++){
      EdgeIter target_edgeIter = oldEdgeBuffer[j];
      Vector3D position_buffer = target_edgeIter->newPosition;
      VertexIter new_vertex = mesh.splitEdge(target_edgeIter);
      new_vertex -> isNew = true;
      new_vertex->position = position_buffer;

      //update the edge information
      // from the split function, we set vertex.halfedge to be h_15, and it is defined as a new vertex, based on the spliting figure
      HalfedgeIter h15 = new_vertex->halfedge();
      h15->edge()->isNew = false; //I set it to be true at first.
      h15->next()->next()->edge()->isNew = true;
      h15->twin()->next()->edge()->isNew = true;
      h15->twin()->next()->twin()->next()->edge()->isNew = false;
    }

    // int total_edges = mesh.nEdges();
    // int split_count = 0;
    // EdgeIter eg = mesh.edgesBegin();
    // while(split_count != total_edges){
    //   if (eg->isNew == false){
    //     VertexIter new_vertex = mesh.splitEdge(eg);
    //     new_vertex -> isNew = true;
    //     new_vertex->position = eg->newPosition;
    //     split_count++;
    //   }else continue;
      
    // }
    std::cout<<"HhhhhhHHHHHHHHHHHHH"<<endl;
    
    // 4. Flip any new edge that connects an old and new vertex.
    for( EdgeIter edg = mesh.edgesBegin(); edg != mesh.edgesEnd(); edg++ )
    {
      if(edg->isNew == true){
        HalfedgeIter h_edg = edg->halfedge();
        HalfedgeIter h_t_edg = h_edg->twin();
        VertexIter v_1 = h_edg->vertex();
        VertexIter v_2 = h_t_edg->vertex();
        if ((v_1->isNew) != (v_2->isNew)){
          edg = mesh.flipEdge(edg);
        }
      }

    }

    // 5. Copy the new vertex positions into final Vertex::position.
    for (VertexIter v_last = mesh.verticesBegin(); v_last != mesh.verticesEnd(); v_last++) {
      if (v_last->isNew == false){
        v_last->position = v_last->newPosition;
      }
    }
    return;
  }
}
