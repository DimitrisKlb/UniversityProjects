/*
          Simple Software 3D Java Rendering Engine
                  (C) Dean Camera, 2007

        dean_camera (at} fourwalledcubicle [dot> com
             http://www.fourwalledcubicle.com
*/

public final class Point
{
   private float   X, Y, Z;
   private int     ScreenX, ScreenY;
   private boolean Recalculate;
   
   public static final Point ORIGIN = new Point(0, 0, 0);
   
   public Point(final float X, final float Y, final float Z)
   {
      this.X = X;
      this.Y = Y;
      this.Z = Z;

      Recalculate = true;
   }

   public void RotateX(final double RadDegrees)
   {
      RotateX((float)Math.sin(RadDegrees), (float)Math.cos(RadDegrees));
   }
      
   public void RotateX(final float SinVal, final float CosVal)
   {
      float NewY = ((CosVal * Y) - (SinVal * Z));
      float NewZ = ((CosVal * Z) + (SinVal * Y));
   
      Y = NewY;
      Z = NewZ;

      Recalculate = true;
   }

   public void RotateY(final double RadDegrees)
   {
      RotateY((float)Math.sin(RadDegrees), (float)Math.cos(RadDegrees));
   }

   public void RotateY(final float SinVal, final float CosVal)
   {
      float NewX = ((CosVal * X) - (SinVal * Z));
      float NewZ = ((CosVal * Z) + (SinVal * X));

      X = NewX;
      Z = NewZ;

      Recalculate = true;
   }

   public void RotateZ(final double RadDegrees)
   {
      RotateZ((float)Math.sin(RadDegrees), (float)Math.cos(RadDegrees));
   }

   public void RotateZ(final float SinVal, final float CosVal)
   {                 
      float NewX = ((CosVal * X) - (SinVal * Y));
      float NewY = ((CosVal * Y) + (SinVal * X));

      X = NewX;
      Y = NewY;

      Recalculate = true;
   }

   public float GetX()
   {
      return X;
   }

   public float GetY()
   {
      return Y;
   }

   public float GetZ()
   {
      return Z;
   }

   public void SetX(final float NewX)
   {
      X  = NewX;

      Recalculate = true;
   }

   public void SetY(final float NewY)
   {
      Y  = NewY;

      Recalculate = true;
   }

   public void SetZ(final float NewZ)
   {
      Z  = NewZ;

      Recalculate = true;
   }

   public int GetScreenX()
   {
      if (Recalculate)
         Recalculate();

      return ScreenX;
   }
   
   public int GetScreenY()
   {
      if (Recalculate)
         Recalculate();

      return ScreenY;
   }

   private void Recalculate()
   {
      float ZScale = (ScreenTrans.FOCAL / (ScreenTrans.FOCAL + Z));

      ScreenX = ((int)(X * ZScale) + ScreenTrans.VP_X);
      ScreenY = ((int)(Y * ZScale) + ScreenTrans.VP_Y);

      Recalculate = false;
   }
}

