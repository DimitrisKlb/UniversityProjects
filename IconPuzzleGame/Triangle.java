/*
          Simple Software 3D Java Rendering Engine
                  (C) Dean Camera, 2007

        dean_camera (at} fourwalledcubicle [dot> com
             http://www.fourwalledcubicle.com
*/

import java.awt.Color;
import java.awt.Polygon;
import java.awt.Graphics2D;

public final class Triangle implements Comparable<Triangle>
{
   private Point[] Points;
   private Color   TColor;
   private int[]   XPoints, YPoints;
     
   public Triangle(final Point P1, final Point P2, final Point P3)
   {
      this(P1, P2, P3, Color.MAGENTA);
   }

   public Triangle(final Point P1, final Point P2, final Point P3, final Color TColor)
   {
      Points = new Point[3];
      
      Points[0] = P1;
      Points[1] = P2;
      Points[2] = P3;

      XPoints = new int[3];
      YPoints = new int[3];

      this.TColor = TColor;
   }

   private void UpdateXYCoordsList(final Point Position)
   {
      int[]   XPoints = this.XPoints;
      int[]   YPoints = this.YPoints;
      Point[] Points  = this.Points;

      float PosX = Position.GetX();
      float PosY = Position.GetY();
      float PosZ = Position.GetZ();

      for (int i = 2; i >= 0; --i)
      {
         float PX = (Points[i].GetX() + PosX);
         float PY = (Points[i].GetY() + PosY);
         float PZ = (Points[i].GetZ() + PosZ);
         
         float ZScale = (ScreenTrans.FOCAL / (ScreenTrans.FOCAL + PZ));

         XPoints[i] = ((int)(PX * ZScale) + ScreenTrans.VP_X);
         YPoints[i] = ((int)(PY * ZScale) + ScreenTrans.VP_X);
      }
   }

   private void UpdateXYCoordsList()
   {
      int[]   XPoints = this.XPoints;
      int[]   YPoints = this.YPoints;
      Point[] Points  = this.Points;
   
      for (int i = 2; i >= 0; --i)
      {
         XPoints[i] = Points[i].GetScreenX();
         YPoints[i] = Points[i].GetScreenY();
      }
   }
  
   public Polygon GetPolygon()
   {
      UpdateXYCoordsList();
      
      return (new Polygon(XPoints, YPoints, 3)); 
   }

   public Polygon GetPolygon(final Point Location)
   {   
      UpdateXYCoordsList(Location);

      return (new Polygon(XPoints, YPoints, 3));
   }

   public void DrawTrianglePoints(final Graphics2D Screen, final Point Location)
   {
      UpdateXYCoordsList(Location);
      
      Screen.setColor(TColor);

      for (int i = 2; i >= 0; --i)
      {
         Screen.drawRect(XPoints[i], YPoints[i], 1, 1);
      }
   }

   public void DrawTriangleWireframe(final Graphics2D Screen, final Point Location)
   {
      UpdateXYCoordsList(Location);

      Screen.setColor(TColor);
      Screen.drawPolyline(XPoints, YPoints, 3);
   }

   public void DrawTriangle(final Graphics2D Screen, final Point Location)
   {
      UpdateXYCoordsList(Location);

      if (IsBackFacing() == false)
      {         
         Screen.setColor(TColor);
         Screen.fillPolygon(XPoints, YPoints, 3);
      }
   }

   public void DrawTrianglePoints(final Graphics2D Screen)
   {
      UpdateXYCoordsList();

      Screen.setColor(TColor);

      for (int i = 2; i >= 0; --i)
      {
         Screen.drawLine(XPoints[i], YPoints[i], XPoints[i], YPoints[i]);
      }
   }

   public void DrawTriangleWireframe(final Graphics2D Screen)
   {
      UpdateXYCoordsList();

      Screen.setColor(TColor);
      Screen.drawPolyline(XPoints, YPoints, 3);
   }

   public void DrawTriangle(final Graphics2D Screen)
   {
      UpdateXYCoordsList();

      if (IsBackFacing() == false)
      {         
         Screen.setColor(TColor);
         Screen.fillPolygon(XPoints, YPoints, 3);
      }
   }

   public void DrawTrianglePoints(final Graphics2D Screen, final Light[] Lights)
   {
      UpdateXYCoordsList();

      Screen.setColor(LightManager.LightTriangle(Lights, Points, TColor));

      for (int i = 2; i >= 0; --i)
      {
         Screen.drawLine(XPoints[i], YPoints[i], XPoints[i], YPoints[i]);
      }
   }

   public void DrawTriangleWireframe(final Graphics2D Screen, final Light[] Lights)
   {
      UpdateXYCoordsList();

      Screen.setColor(LightManager.LightTriangle(Lights, Points, TColor));
      Screen.drawPolyline(XPoints, YPoints, 3);
   }

   public void DrawTriangle(final Graphics2D Screen, final Light[] Lights)
   {
      UpdateXYCoordsList();

      if (IsBackFacing() == false)
      {         
         Screen.setColor(LightManager.LightTriangle(Lights, Points, TColor));
         Screen.fillPolygon(XPoints, YPoints, 3);
      }
   }

   public void DrawTrianglePoints(final Graphics2D Screen, final Point Location, final Light[] Lights)
   {
      UpdateXYCoordsList(Location);

      Screen.setColor(LightManager.LightTriangle(Lights, Location, Points, TColor));

      for (int i = 2; i >= 0; --i)
      {
         Screen.drawLine(XPoints[i], YPoints[i], XPoints[i], YPoints[i]);
      }
   }

   public void DrawTriangleWireframe(final Graphics2D Screen, final Point Location, final Light[] Lights)
   {
      UpdateXYCoordsList(Location);

      Screen.setColor(LightManager.LightTriangle(Lights, Location, Points, TColor));
      Screen.drawPolyline(XPoints, YPoints, 3);
   }

   public void DrawTriangle(final Graphics2D Screen, final Point Location, final Light[] Lights)
   {
      UpdateXYCoordsList(Location);

      if (IsBackFacing() == false)
      {         
         Screen.setColor(LightManager.LightTriangle(Lights, Location, Points, TColor));
         Screen.fillPolygon(XPoints, YPoints, 3);
      }
   }

   public void SetColor(final Color NewColor)
   {
      TColor = NewColor;
   }
   
   public Color GetColor()
   {
      return TColor;
   }

   public Point[] GetPoints()
   {
      return Points;
   }

   private boolean IsBackFacing()
   {
      int[] XPoints = this.XPoints;
      int[] YPoints = this.YPoints;

      float CaX = XPoints[2] - XPoints[0];
      float CaY = YPoints[2] - YPoints[0];
      
      float BcX = XPoints[1] - XPoints[2];
      float BcY = YPoints[1] - YPoints[2];

      return ((CaX * BcY) < (CaY * BcX));
   }

   public float GetZSum()
   {
      Point[] Points = this.Points;

      return (Points[0].GetZ() + Points[1].GetZ() + Points[2].GetZ());
   }

   public int compareTo(final Triangle T2)
   {
      float T1ZSum, T2ZSum;

      T1ZSum = GetZSum();
      T2ZSum = T2.GetZSum();

      if (T1ZSum > T2ZSum)
         return 1;
      else if (T1ZSum < T2ZSum)
         return -1;
      else
         return 0;
   }
}

