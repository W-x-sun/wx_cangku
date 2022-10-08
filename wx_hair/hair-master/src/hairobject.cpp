#include "hairobject.h"

#include "hair.h"
#include "errorchecker.h"
#include "simulation.h"
#include "texture.h"
#include "blurrer.h"

#include <glm/gtx/color_space.hpp>

HairObject::~HairObject()
{
    for (int i = 0; i < m_guideHairs.size(); ++i)
        delete m_guideHairs.at(i);
    safeDelete(m_blurredHairGrowthMapTexture);
}

HairObject::HairObject(
        ObjMesh *mesh,
        float hairsPerUnitArea,//头发密度
        float maxHairLength,//头发长度
        QImage &hairGrowthMap,//头发生长图（自己刷子画的）赋值给m_hairGrowthMap再赋值给对应纹理进行应用
        QImage &hairGroomingMap,//（渲染窗口Direction Map的背景图）头发修饰图  赋值给m_hairGroomingMap再赋值给对应纹理进行应用
        Simulation *simulation,//用于仿真参数加载
        HairObject *oldObject,
        int _face)
{    
    if (hairGrowthMap.width() == 0)
    {
        std::cout << "Hair growth map does not appear to be a valid image." << std::endl;
        exit(1);
    }

    m_hairGrowthMap = hairGrowthMap;//头发生长图，从子窗口传进来的纹理图片
    m_hairGroomingMap = hairGroomingMap;//头发修饰图

    // 初始化（blurred）头发生长纹理
    QImage blurredImage;
    Blurrer::blur(hairGrowthMap, blurredImage);
    m_blurredHairGrowthMapTexture = new Texture();
    m_blurredHairGrowthMapTexture->createColorTexture(blurredImage, GL_LINEAR, GL_LINEAR);//生成纹理 并设置了纹理参数

    
    int _failures = 0;
    int _emptyPoints = 0;
    for (unsigned int i = 0; i < mesh->triangles.size(); i++)//这个三角形在objmesh：：init（）里完成了初始化5946
    {
        //0-3000为右前半部分
        //4000-5946为完整左半部分
        //3000-4000为剩下的部分（右后半部分）
//        std::cout<<" mesh->triangles.size()为"<< mesh->triangles.size()<<endl;
        Triangle t = mesh->triangles[i];

        //在（单个）三角形上生成 头发
        int numHairs = (int) (hairsPerUnitArea * t.area() + rand() / (float)RAND_MAX);//密度乘上（单个三角形）面积得到头发数量
        for (int hair = 0; hair < numHairs; hair++)//对每根头发
        {
            // 在三角形上生成随机点
            glm::vec3 pos; glm::vec2 uv; glm::vec3 normal;
            //可通过设置randpoint（）函数来设置头发ROI区域
            t.randPoint(pos, uv, normal, _face);   //参数 ：表示输出，在三角形中生成随机点pos、随机点对应的uv和normal

            uv = glm::vec2(MIN(uv.x, 0.999), MIN(uv.y, 0.999)); // Make UV in range [0,1) instead of [0,1]

            // 得到对应三角形内部某个点的vt在纹理贴图上的像素坐标位置       (hairGrowthMap对应":/images/headHair.jpg"或手绘的)
            QPoint p = QPoint(uv.x * hairGrowthMap.width(), (1 - uv.y) * hairGrowthMap.height());      //1-uv.y的原因是 为了消除纹理坐标（原点左下角）与图像像素坐标（原点左上）之间y的效果翻转

//            pos = glm::vec3(MIN((pos.x+1)/2 , 0.999), MIN(pos.y , 0.), MIN((pos.z+1)/2, 0.999));
//            QPoint p = QPoint((pos.x) * hairGrowthMap.width(), (pos.z) * hairGrowthMap.height());
/*****************************************************************************************************************
           为abs(pos.x)则为两侧对称生成,-(pos.x)为另一侧   xy轴组成的平面来映射显示，所以pos.z的正负不受影响（z的正负轴同时生成头发）
//              QPoin //——————————————————————————修改代码（利用随机点的顶点坐标）———————————————————————————————//
              pos = glm::vec3(MIN(pos.x, 0.999), MIN(pos.y, 0.999),MIN(pos.z, 0.999));//得有z轴参数(若abs(z),则可取消z对称效果,但右半部分没法涂抹)

              //若t p = QPoint((pos.x) * hairGrowthMap.width(), (1 - pos.y) * hairGrowthMap.height());//1-y：为了使植发位置和涂抹区域对应
                //width()*2可以弥补后端不能涂抹的问题
              //——————— —————— —————— ————— ————— ————— ————— ———— —————— ————— ———— —————— //
                            //xyz三个分量不应该为0
//              pos = glm::vec3(MIN(pos.x, 0.999), MIN(pos.y, 0.999),MIN(pos.z, 0.999));//得有z轴参数(若abs(z),则可取消z对称效果,但右半部分没法涂抹)

//              //若为abs(pos.x)则为两侧对称生成   xy轴组成的平面来映射显示，所以pos.z的正负不受影响（z的正负轴同时生成头发）
//              QPoint p = QPoint((pos.x) * hairGrowthMap.width()*2, (1-pos.y) * hairGrowthMap.height());//1-y：为了使植发位置和涂抹区域对应
//            //width()*2可以弥补后端不能涂抹的问题
              //——————— —————— —————— ————— ————— ————— ————— ———— —————— ————— ———— —————— //
*****************************************************************************************************************/

               //先判断点p是否有效
              if (!hairGrowthMap.valid(p)){
                _failures++;
                continue;  // 继续对下一根头发处理
            }
            //1.判断点p在是 hairGrowthMap 上否是有用的像素点
            QColor hairGrowth = QColor(hairGrowthMap.pixel(p));
            if (hairGrowth.valueF() < 0.05){// 如果头发生长图是黑色的（背景色，没有在模型上用刷子刷图），跳过
                _emptyPoints++;
                continue;// 继续对下一根头发处理
            }
            
            glm::vec3 u = glm::normalize(glm::cross(normal, glm::vec3(0, 1, 0)));
            glm::vec3 v = glm::normalize(glm::cross(u, normal));
            glm::mat3 m = glm::mat3(u, v, normal);//坐标

            //2.判断点p在 m_hairGroomingMap 是上否是有用的像素点
            QColor groomingColor = QColor(m_hairGroomingMap.pixel(p));//得到uv坐标对应的头发修饰图对应的像素颜色
            float a = 10.0 * (groomingColor.red() - 128.0) / 255.0;
            float b = 10.0 * (groomingColor.green() - 128.0) / 255.0;
            glm::vec3 x = glm::vec3(a, b, 1.0);//颜色


            glm::vec3 dir = glm::normalize(m * x);

//            poses.append(pos);
//            normals.append(normal);
//            dirs.append(dir);

            //new hair这个类构造函数里生成 ：每根头发的数据（结构体），将所有头发数据（结构体）存储到 m_vertices列表，被paint（）调用
            m_guideHairs.append(new Hair(20, maxHairLength * hairGrowth.valueF(), pos, dir, normal));
            //m_guideHairs里放的元素为 单个三角形（也就是每一撮头发），所以hair类存储数据是以三角网格（撮）为单位的
        }

//        for(i = 0; i < poses.size(); i++){
////            pos = poses.at(i);
//            std::cout<<i<<endl;
//            //new hair这个类构造函数里生成 ：每根头发的数据（结构体），将所有头发数据（结构体）存储到 m_vertices列表，被paint（）调用
//            m_guideHairs.append(new Hair(20, maxHairLength * hairGrowth.valueF(), poses.at(i), dirs.at(i), normals.at(i)));
//            //m_guideHairs里放的元素为 单个三角形（也就是每一撮头发），所以hair类存储数据是以三角网格（撮）为单位的
//        }

    }
    
    setAttributes(oldObject);//给上个设置参数

    m_simulation = simulation;    
}
//给头发的属性设置（若传入上一个头发的参数，应该是指场景设置暂停之前的属性）
void HairObject::setAttributes(HairObject *_oldObject){
    //先判断之前是否已经有了使用过的参数
    if (_oldObject == NULL){//若没有
        setAttributes();//调用此函数，没有传进参数，则使用初始化参数（初始化参数在头文件定义里直接给了，看定义）
    } else {//如果有
        setAttributes(//把当前窗口的参数数值给传进去  并赋值给_oldObject
                    _oldObject->m_color,//
                    _oldObject->m_numGroupHairs,
                    _oldObject->m_hairGroupSpread,
                    _oldObject->m_hairRadius,
                    _oldObject->m_noiseAmplitude,
                    _oldObject->m_noiseFrequency,
                    _oldObject->m_numSplineVertices,
                    _oldObject->m_shadowIntensity,//
                    _oldObject->m_diffuseIntensity,//
                    _oldObject->m_specularIntensity,//
                    _oldObject->m_transparency,//
                    _oldObject->m_useHairColorVariation,//渲染模块 的颜色复选框
                    _oldObject->m_hairColorVariation);//
    }
}
//相当于初始化设置头发的属性   参数值给定的    （颜色、半径... ...）
void HairObject::setAttributes(glm::vec3 _color, int _numGroupHairs, float _hairGroupSpread, float _hairRadius, float _noiseAmplitude, float _noiseFrequency, int _numSplineVertices, float _shadowIntensity, float _diffuseIntensity, float _specularIntensity, float _transparency, float _useHairColorVariation, float _hairColorVariation){
    m_color = _color;
    m_numGroupHairs = _numGroupHairs;//下边等式右边的都是初始化参数（值给定的）
    m_hairGroupSpread = _hairGroupSpread;
    m_hairRadius = _hairRadius;
    m_noiseAmplitude = _noiseAmplitude;
    m_noiseFrequency = _noiseFrequency;
    m_numSplineVertices = _numSplineVertices;
    m_shadowIntensity = _shadowIntensity;
    m_diffuseIntensity = _diffuseIntensity;
    m_specularIntensity = _specularIntensity;
    m_transparency = _transparency;
    m_useHairColorVariation = _useHairColorVariation;
    m_hairColorVariation = _hairColorVariation;
}

void HairObject::update(float _time){

    if (m_simulation != NULL)
    {
        m_simulation->simulate(this);//this指的是这个hairobject类指针对象  实现仿真实现
    }

    for (int i = 0; i < m_guideHairs.size(); i++)
    {
        m_guideHairs.at(i)->update(_time);
    }

}

//依据 几何模块（6参数）、渲染模块（color参数）参数的对应设定值，使用着色器，进行每一撮头发的绘制paint
void HairObject::paint(ShaderProgram *program){
    
    program->uniforms.color = glm::rgbColor(glm::vec3(m_color.x*255, m_color.y, m_color.z));
    //几何模块参数传入到着色器程序的全局数据中
    program->uniforms.numGroupHairs = m_numGroupHairs;
    program->uniforms.hairGroupSpread = m_hairGroupSpread;
    program->uniforms.hairRadius = m_hairRadius;
    program->uniforms.noiseAmplitude = m_noiseAmplitude;
    program->uniforms.noiseFrequency = m_noiseFrequency;
    program->uniforms.numSplineVertices = m_numSplineVertices;
    program->setPerObjectUniforms();// 设置发生变化的对象之间的uniforms.
     //对每一撮（每一个三角网格）头发进行绘制
    for (int i = 0; i < m_guideHairs.size(); i++)
    {
        m_guideHairs.at(i)->paint(program);//此函数作用域为Hair::paint，
    }

}
