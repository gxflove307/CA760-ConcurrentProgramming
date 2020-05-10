  
#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#define VECTOR_SIZE 1024

public void Compute_cl(string imageFile)
{
    //选取设备
    var platform = ComputePlatform.Platforms.FirstOrDefault();
    var device = platform.Devices.FirstOrDefault();
    //设置相关上下文
    var properties = new ComputeContextPropertyList(platform);
    var context = new ComputeContext(new[] {device}, properties, null, IntPtr.Zero);
    //命令队列，用于控制执行的代码
    ComputeCommandQueue commands = new ComputeCommandQueue(context, context.Devices[0],
        ComputeCommandQueueFlags.None);
    //读取opencl代码
    var code = File.ReadAllText(@"Sobel_edge_detection_practice.cl");
    //编译
    var program = new ComputeProgram(context, code);
    try
    {
        program.Build(new[] {device}, null, null, IntPtr.Zero);
    }
    catch (Exception ex)
    {
        throw;
    }
    var images = CreateImageFromBitmap(imageFile, context,
        ComputeMemoryFlags.ReadWrite | ComputeMemoryFlags.CopyHostPointer);
    //创建核心代码，就是cl代码中以kernel标识的函数
    var kernel = program.CreateKernel("gaussian_blur");
    //矩阵规模
    //储存计算结果的数组
    
    //创建的核心代码函数以这种方式来传参
    var resultBuffer=new ComputeBuffer<char>(context,ComputeMemoryFlags.WriteOnly, dstBytes.Length);
    kernel.SetMemoryArgument(0, images);
    kernel.SetMemoryArgument(1, resultBuffer);
    kernel.SetMemoryArgument(2, new ComputeBuffer<float>(context,ComputeMemoryFlags.ReadOnly|ComputeMemoryFlags.CopyHostPointer,_matrix));
    kernel.SetValueArgument(3, Radius);
    kernel.SetValueArgument(4, (int)images.Width);
    Console.WriteLine($"运行平台: {platform.Name}\n运行设备： {device.Name}\n");
    Stopwatch sw = Stopwatch.StartNew();
    var climg = images;
    //执行代码
    commands.Execute(kernel, null, new long[] {climg.Width, climg.Height}, null, null);
    
    //read data
    char[] resultArray = new char[dstBytes.Length];
    var arrHandle = GCHandle.Alloc(resultArray, GCHandleType.Pinned);
    commands.Read(resultBuffer, true, 0, dstBytes.Length, arrHandle.AddrOfPinnedObject(), null);
    //commands.ReadFromImage(images.Item2, processeddata.Scan0, true, null);
    var resultHandle = GCHandle.Alloc(resultArray, GCHandleType.Pinned);
    var bmp=new Bitmap(climg.Width,climg.Height, climg.Width*4, PixelFormat.Format32bppArgb, resultHandle.AddrOfPinnedObject());
    var elapsed = sw.Elapsed;
    Console.WriteLine($"耗时: {elapsed.TotalMilliseconds} ms\n");
    kernel.Dispose();
    bmp.Save("processed_cl.bmp");
}
