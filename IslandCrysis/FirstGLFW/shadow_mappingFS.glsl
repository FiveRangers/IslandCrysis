#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float near_plane;
uniform float far_plane;

uniform bool isOrtho;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    //执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //归一化
    projCoords = projCoords * 0.5 + 0.5;
    //得到光源视角最近的深度值
    float closestDepth;
    float currentDepth;

    if (isOrtho) {
    	closestDepth = texture(shadowMap, projCoords.xy).r;
    	currentDepth = projCoords.z;
		//计算偏移量
	    vec3 normal = normalize(fs_in.Normal);
	    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	    //PCF
	    float shadow = 0.0;
	    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	    for(int x = -1; x <= 1; ++x)
	    {
	        for(int y = -1; y <= 1; ++y)
	        {
	            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
	            //判断哪个大
	            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
	        }    
	    }
	    shadow /= 9.0;
	    //强制把shadow的值设为0.0，不管投影向量的z坐标是否大于1.0
    	if(projCoords.z > 1.0)
        	shadow = 0.0;
	    return shadow;
    } else {
    	//如果是透视投影，就将深度值改为线性的
	    closestDepth = texture(shadowMap, projCoords.xy).r;
	    closestDepth = LinearizeDepth(closestDepth);
	    //得到视角当前深度
	    currentDepth = projCoords.z;
	    currentDepth = LinearizeDepth(currentDepth);

	    float shadow = 0.0;
	    //判断哪个大
	    shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

	    //强制把shadow的值设为0.0，不管投影向量的z坐标是否大于1.0
    	if(projCoords.z > 1.0)
        	shadow = 0.0;
	    return shadow;
	}
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    //环境光照，这里设为1
    vec3 ambient = color;
    //漫反射
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    //镜面反射
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    //计算是否在阴影中
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    
    //根据是否在阴影中决定是否只有环境光照
    FragColor = vec4(lighting, 1.0);

    
}