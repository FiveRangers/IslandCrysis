#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal; 
in vec2 TexCoords;

uniform sampler2D sampler;
float shininess = 64.0;

vec3 lightPosition = vec3(200.0f, 200.0f, -100.0f);

uniform vec3 viewPos;

void main()
{    
	// ambient
	vec3 ambient = 0.35f * texture(sampler, TexCoords).rgb;

	// diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.5f * diff * texture(sampler, TexCoords).rgb;

	// specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = 0.0f * spec * texture(sampler, TexCoords).rgb;  

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}