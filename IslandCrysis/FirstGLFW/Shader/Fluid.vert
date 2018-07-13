#version 330 core
layout (location=0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float time;
uniform vec4 color;

out vec4 FluidColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
  FluidColor=color;
  // 近似将海面的法向量都等于（0,1,0）
  FragPos = vec3(model * vec4(aPos, 1.0));
  Normal=mat3(transpose(inverse(model))) * vec3(0.0f,1.0f,0.0f);

  // 传播方向
  float thetas[2]={0.38,1.42};
  // 角频率
  float omegas[3]={3.27,3.31,3.42};
  // 波数
  float ks[3]={0.091,0.118,0.1935};
  // 振幅
  float amplitudes[6] = {
        0.2,0.2,
        0.3,0.5,
        0.2,0.6
  };

  float x=aPos.x;
  float y=aPos.y;
  float z=aPos.z;


  for(int i=0;i<3;i++)
  {
    for(int j=0;j<2;j++)
    {
      x=x-cos(thetas[j])*amplitudes[i*2+j]*sin(ks[i]*(aPos.x*cos(thetas[j])+
        aPos.z*sin(thetas[j]))-omegas[i]*time);
      y=y+amplitudes[i*2+j]*cos(ks[i]*(aPos.x*cos(thetas[j])+aPos.z*sin(thetas[j]))-omegas[i]*time);
      z=z-sin(thetas[j])*amplitudes[i*2+j]*sin(ks[i]*(aPos.x*cos(thetas[j])+aPos.z*sin(thetas[j]))-omegas[i]*time);
    }
  }

  TexCoord=vec2(aPos.x/50,aPos.z/50);
  gl_Position=projection*view*model*vec4(x,y,z,1.0);
}
