#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
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
    //ִ��͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //��һ��
    projCoords = projCoords * 0.5 + 0.5;
    //�õ���Դ�ӽ���������ֵ
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    /*if (isOrtho) {
		//����ƫ����
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
	            //�ж��ĸ���
	            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
	        }    
	    }
	    shadow /= 9.0;
	    //ǿ�ư�shadow��ֵ��Ϊ0.0������ͶӰ������z�����Ƿ����1.0
    	if(projCoords.z > 1.0)
        	shadow = 0.0;
	    return shadow;
    } else {
    	//�����͸��ͶӰ���ͽ����ֵ��Ϊ���Ե�
	    closestDepth = texture(shadowMap, projCoords.xy).r;
	    closestDepth = LinearizeDepth(closestDepth);
	    //�õ��ӽǵ�ǰ���
	    currentDepth = projCoords.z;
	    currentDepth = LinearizeDepth(currentDepth);

	    float shadow = 0.0;
	    //�ж��ĸ���
	    shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

	    //ǿ�ư�shadow��ֵ��Ϊ0.0������ͶӰ������z�����Ƿ����1.0
    	if(projCoords.z > 1.0)
        	shadow = 0.0;
	    return shadow;
	}*/
	//vec3 normal = normalize(fs_in.Normal);
	//vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float bias = 0.002;
	//PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
	    for(int y = -1; y <= 1; ++y)
	    {
	        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
	        //�ж��ĸ���
	        shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
	    }    
    }
	shadow /= 9.0;

    return shadow;
}

void main()
{           
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);

    //�������գ�������Ϊ1
    vec3 ambient = 0.2 * color;

    //������
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = 0.6 * diff * lightColor;

    //���淴��
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = 0 * spec * lightColor;    

    //�����Ƿ�����Ӱ��
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    
    //�����Ƿ�����Ӱ�о����Ƿ�ֻ�л�������
    FragColor = vec4(lighting, 1.0);

    
}