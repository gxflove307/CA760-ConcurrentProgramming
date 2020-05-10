/*
定义一个图片像素采样器。
有三个参数：
CLK_NORMALIZED_COORDS_FALSE（是否标准化坐标）
CLK_FILTER_NEAREST（滤波器）
CLK_ADDRESS_CLAMP_TO_EDGE（当坐标转移到图片最边缘一个像素时的采样方式，这里是设置成采用图片边缘的像素作为采样点）
*/
__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP_TO_EDGE;

/*
定义一个阈值，当计算后的像素值超过这个数字时，就全黑，小于这个值时，为全白
这个依据图片处理算法而定，这里的125只是一个🌰
*/
#define THRESHOLD 125

// 入参in：为image2d_t型的图片，这里说明这是一个2D图片；出参out：也是一个image2d_t型的图片，这里说明kernel函数返回的也是一个2D图片
__kernel void opKernel(__read_only image2d_t in, int op, __write_only image2d_t out)
{
  //获取2个work-item工作单元，它们能按坐标位置操作数据
  const int x = get_global_id(0);
  const int y = get_global_id(1);
  
  const float pxVal_1 = read_imagef(in, sampler, (int2)(x-1,y+1)).s0;//通过采样器，获取坐标为(x-1,y+1)或者说是(i-1,j+1)位置的像素值，
									//read_imagef()函数会返回多个值，这里只取第一个值（即像素点的值）就可以了
  const float pxVal_2 = read_imagef(in, sampler, (int2)(x-1,y-1)).s0;//同上
  const float pxVal_3 = read_imagef(in, sampler, (int2)(x,y+1)).s0;
  const float pxVal_4 = read_imagef(in, sampler, (int2)(x,y-1)).s0;
  const float pxVal_5 = read_imagef(in, sampler, (int2)(x+1,y+1)).s0;
  const float pxVal_6 = read_imagef(in, sampler, (int2)(x+1,y-1)).s0;
  const float pxVal_7 = read_imagef(in, sampler, (int2)(x-1,y)).s0;
  const float pxVal_8 = read_imagef(in, sampler, (int2)(x+1,y)).s0;
  
  float value_1 = pxVal_1 - pxVal_2 + 2*pxVal_3 - pxVal_4 + pxVal_5 - pxVal_6;//没什么好说的，纯数学计算
  float value_2 = pxVal_1 +2*pxVal_7 + pxVal_2 -pxVal_5 -2*pxVal_8 - pxVal_6;
  
  float square_sum = pown(value_1,2) + pown(value_2,2);//取平方值
  
  float pxResult = square_sum > THRESHOLD ? 255.0f : 0.0f;
  
  write_imagef(out, (int2)(x,y), (float4)(pxResult, 0.0f, 0.0f,0.0f));//把计算后的像素值存入输出图像中所对应的坐标位置
}
