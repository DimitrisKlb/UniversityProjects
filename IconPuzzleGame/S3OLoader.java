/*
          Simple Software 3D Java Rendering Engine
                  (C) Dean Camera, 2007

        dean_camera (at} fourwalledcubicle [dot> com
             http://www.fourwalledcubicle.com
*/

import java.io.RandomAccessFile;

public abstract class S3OLoader
{
   private static RandomAccessFile br;

   private static int toInt()
   {
      try
      {
         return (int)((long)(br.read() & 0xFF) | (long)(br.read() & 0xFF) <<
         8 | (long)(br.read() & 0xFF) << 16 | (long)(br.read() & 0xFF) << 24);
      } catch (Exception e)
      {
         return 0;
      }
   }

   private static float toFloat()
   {
      int bits;

      try
      {
         bits = toInt();
      }
      catch (Exception e)
      {
         return 0;
      }

      int s = ((bits >> 31) == 0) ? 1 : -1;
      int e = ((bits >> 23) & 0xff);
      int m = (e == 0) ? (bits & 0x7fffff) << 1 :
                         (bits & 0x7fffff) | 0x800000;
      return (float)s * (float)m * (float)Math.pow(2, e - 150);
   }

   public static Shape3D LoadS3OFile(String FileName)
   {
      int Version;
      int DataOffset;
      int NumPoints;
      int NumFaces;

      Point ShapePoints[];
      Triangle ShapeTriangles[];

      Shape3D NewShape;

      try
      {
         br = new RandomAccessFile(FileName, "r");

         System.out.println("  # Loading " + FileName + "...");
      } catch (Exception e)
      {
         System.out.println("  # Cannot Load " + FileName + "!");

         return null;
      }

      try
      {
         String magic = "" + (char)br.read() + (char)br.read() + (char)br.read();

         if(!magic.equals("S3O"))
         {
            System.out.println("    Magic number not found!");
            return null;
         }

         br.skipBytes(1);

         Version = (int)br.read();
         System.out.println("     > Version:    " + Version);

         DataOffset = (int)br.read();
         System.out.println("     > DataOffset: " + DataOffset);

         NumPoints  = toInt();
         System.out.println("     > NumPoints:  " + NumPoints);

         NumFaces   = toInt();
         System.out.println("     > NumFaces:   " + NumFaces);

         ShapePoints = new Point[NumPoints];
         ShapeTriangles = new Triangle[NumFaces];

         for(int i = 0; i < NumPoints; ++i)
         {
            ShapePoints[i] = new Point(toFloat(), toFloat(), toFloat());
         }

         for(int i = 0; i < NumFaces; ++i)
         {
            ShapeTriangles[i] = new Triangle(ShapePoints[toInt()], ShapePoints[toInt()], ShapePoints[toInt()]);
         }

         NewShape = new Shape3D(ShapePoints, ShapeTriangles);

         return NewShape;
      } catch (Exception e)
      {
         return null;
      }
   }
}
