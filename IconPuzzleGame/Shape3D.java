/*
          Simple Software 3D Java Rendering Engine
                  (C) Dean Camera, 2007

        dean_camera (at} fourwalledcubicle [dot> com
             http://www.fourwalledcubicle.com
*/

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.Arrays;
import java.util.Comparator;

public final class Shape3D implements Comparable<Shape3D>
{
   private Point    PointList[];
   private Triangle TriangleList[];
   
   private Point    Location;

   public Shape3D(final Point[] PointList, final Triangle[] TriangleList, final Point Location, final Color[] ColorList)
   {
      this(PointList, TriangleList, Location);

      ColorShape(ColorList);
   }
   
   public Shape3D(final Point[] PointList, final Triangle[] TriangleList)
   {
      this(PointList, TriangleList, Point.ORIGIN);
   }
   
   public Shape3D(final Point[] PointList, final Triangle[] TriangleList, final Point Location)
   {
      this.Location = Location;
      this.PointList = PointList;
      this.TriangleList = TriangleList;
   }

   public Point[] GetPoints()
   {
      return PointList;
   }

   public Triangle[] GetTriangles()
   {
      return TriangleList;
   }

   public void ColorShape(final Color[] ColorList)
   {
      Triangle[] TriangleList = this.TriangleList;

      for (int i = 0; (i < ColorList.length) && (i < TriangleList.length); i++)
      {
         TriangleList[i].SetColor(ColorList[i]);
      }
   }

   public void SetLocation(final Point NewLocation)
   {
      Location = NewLocation;
   }

   public Point GetLocation()
   {
      return Location;
   }

   public void RotatePointsX(final double RadDegrees)
   {
      RotatePointsX((float)Math.sin(RadDegrees), (float)Math.cos(RadDegrees));
   }

   public void RotatePointsX(final float SinVal, final float CosVal)
   {
      Point[] PointList = this.PointList;
      
      for (int i = (PointList.length - 1); i >= 0; --i)
      {
         PointList[i].RotateX(SinVal, CosVal);
      }
   }

   public void RotatePointsY(final double RadDegrees)
   {
      RotatePointsY((float)Math.sin(RadDegrees), (float)Math.cos(RadDegrees));
   }

   public void RotatePointsY(final float SinVal, final float CosVal)
   {
      Point[] PointList = this.PointList;
      
      for (int i = (PointList.length - 1); i >= 0; --i)
      {
         PointList[i].RotateY(SinVal, CosVal);
      }
   }

   public void RotatePointsZ(final double RadDegrees)
   {
      RotatePointsZ((float)Math.sin(RadDegrees), (float)Math.cos(RadDegrees));
   }

   public void RotatePointsZ(final float SinVal, final float CosVal)
   {
      Point[] PointList = this.PointList;
      
      for (int i = (PointList.length - 1); i >= 0; --i)
      {
         PointList[i].RotateZ(SinVal, CosVal);
      }
   }

   public float GetAverageZDepth()
   {
      Triangle[] TriangleList = this.TriangleList;
      float      Average      = 0;
	  
      for (int i = (TriangleList.length - 1); i >= 0; --i)
      {
         Average += TriangleList[i].GetZSum();
      }

      Average /= (TriangleList.length * 3);	  

      return Average;
   }

   public void DrawShape(final Graphics2D Screen, final Light[] Lights)
   {
      Triangle[] TriangleList = this.TriangleList;
      Point      Location     = this.Location;

      Arrays.sort(TriangleList);

      for (int i = (TriangleList.length - 1); i >= 0; --i)
      {
         TriangleList[i].DrawTriangle(Screen, Location, Lights);
      }
   }

   public void DrawShapeWireframe(final Graphics2D Screen, final Light[] Lights)
   {
      Triangle[] TriangleList = this.TriangleList;
      Point      Location     = this.Location;
      
      Arrays.sort(TriangleList);

      for (int i = (TriangleList.length - 1); i >= 0; --i)
      {
         TriangleList[i].DrawTriangleWireframe(Screen, Location, Lights);
      }
   }

   public void DrawShapePoints(final Graphics2D Screen, final Light[] Lights)
   {
      Triangle[] TriangleList = this.TriangleList;
      Point      Location     = this.Location;
      
      Arrays.sort(TriangleList);

      for (int i = (TriangleList.length - 1); i >= 0; --i)
      {
         TriangleList[i].DrawTrianglePoints(Screen, Location, Lights);
      }
   }

   public void DrawShape(final Graphics2D Screen)
   {
      Triangle[] TriangleList = this.TriangleList;
      Point      Location     = this.Location;

      Arrays.sort(TriangleList);

      for (int i = (TriangleList.length - 1); i >= 0; --i)
      {
         TriangleList[i].DrawTriangle(Screen, Location);
      }
   }

   public void DrawShapeWireframe(final Graphics2D Screen)
   {
      Triangle[] TriangleList = this.TriangleList;
      Point      Location     = this.Location;

      Arrays.sort(TriangleList);

      for (int i = (TriangleList.length - 1); i >= 0; --i)
      {
         TriangleList[i].DrawTriangleWireframe(Screen, Location);
      }
   }

   public void DrawShapePoints(final Graphics2D Screen)
   {
      Triangle[] TriangleList = this.TriangleList;
      Point      Location     = this.Location;
      
      Arrays.sort(TriangleList);

      for (int i = (TriangleList.length - 1); i >= 0; --i)
      {
         TriangleList[i].DrawTrianglePoints(Screen, Location);
      }
   }

   public int compareTo(final Shape3D S2)
   {
      float S1Z, S2Z;

      S1Z = GetAverageZDepth();
      S2Z = S2.GetAverageZDepth();

      if (S1Z < S2Z)
         return 1;
      else if (S1Z < S2Z)
         return -1;
      else
         return 0;
   }
}
