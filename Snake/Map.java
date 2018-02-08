public class Map{
    private int[][] theMap;
    private int size;
    
    public static final int SPECIAL       =-1;  
    public static final int EMPTY         = 0;     
    public static final int WARP          = 2;
    public static final int WALL          = 3;
    
    public static final int SNAKE         = 11;
    public static final int BRANCH_UP     = 16;
    public static final int BRANCH_RIGHT  = 17;
    public static final int BRANCH_DOWN   = 18;
    public static final int BRANCH_LEFT   = 19;
    
    public static final int WARP_UP       = 26;
    public static final int WARP_RIGHT    = 27;
    public static final int WARP_DOWN     = 28;
    public static final int WARP_LEFT     = 29;
    
    public static final int FOOD          = 31;
    
    public Map(int mapHeight,int mapWidth,int panSize){
        this.theMap = new int[mapHeight][mapWidth];
        this.size = panSize;
    }
    
    public void setCell(int height,int width,int material){
        int d = (size-1)/2;
        for(int h=height-d; h<=height+d; h++){
            for(int w=width-d; w<=width+d; w++){
                theMap[h][w] = material; 
            }
        }      
    } 
    public void setSquare(int height,int width,int material){
        theMap[height][width] = material;                
    } 
    public int cell(int mapH,int mapW){ return this.theMap[mapH][mapW];}     
    public int mHeight(){return theMap.length;}
    public int mWidth(){return theMap[1].length;} 
}