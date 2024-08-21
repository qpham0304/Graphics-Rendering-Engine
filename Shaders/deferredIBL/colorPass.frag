#version 460 core

struct Light {
    vec3 position;
	vec3 color;
	int radius;
};

const int MAX_NUM_LIGHTS = 100;
const float PI = 3.14159265359;


out vec4 FragColor;

in vec2 uv;

uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalRoughness;
uniform sampler2D gEmissive;
uniform sampler2D gDUV;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform Light lights[MAX_NUM_LIGHTS];
uniform mat4 inverseView;
uniform mat4 invProjection;
uniform bool gamma;
uniform vec3 camPos;


//------------------------//
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
} 

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


vec3 getViewSpacePosition(float z) {
    vec2 posCanonical  = uv * 2 - 1.0; //position in Canonical View Volume
	vec4 posView = invProjection * vec4(posCanonical, z , 1.0);
	posView /= posView.w;
	return posView.xyz;
}


vec4 calcLighting() {
    float depthMap = texture(gDepth, uv).r;
    vec3 normal = texture(gNormal, uv).rgb;
    normal = mat3(inverseView) * normal;
    vec3 albedo = pow(texture(gAlbedo, uv).rgb, vec3(2.2));
    float metallic = texture(gMetalRoughness, uv).b;
    float roughness = texture(gMetalRoughness, uv).g;
    float ao = texture(gAlbedo, uv).a;
	vec3 emissive = texture(gEmissive, uv).rgb;

	float depth = texture(gDepth, uv).r * 2 - 1;
	vec3 viewSpacePosition = getViewSpacePosition(depth);
    vec3 worldSpacePosition = mat3(inverseView) * viewSpacePosition;

    vec3 N = normal;
    vec3 V = normalize(worldSpacePosition);
    vec3 R = reflect(V, N); 
	
	vec3 F0 = vec3(0.04);	//0.04 realistic for most dielectric surfaces
	F0 = mix(F0, albedo, metallic);
	
	vec3 Lo = vec3(0.0);
    vec3 lighting = vec3(0.0);
	for(int i = 0; i < MAX_NUM_LIGHTS; i++) {
		// vec3 L = normalize(lights[i].position - worldSpacePosition);
		// vec3 H = normalize(V + L);

		// float distance = length(lights[i].position - worldSpacePosition);
		// float attenuation = 1.0 / (distance * distance);
		// vec3 radiance = lights[i].color * attenuation;
		
		// //BRDF
		// vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);
		// float NDF = DistributionGGX(N, H, roughness);       
		// float G   = GeometrySmith(N, V, L, roughness);

		// vec3 numerator    = NDF * G * F;
		// float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001; // 0.0001 to prevent zero division
		// vec3 specular     = numerator / denominator; 

		// vec3 kS = F;
		// vec3 kD = vec3(1.0) - kS;
		// kD *= 1.0 - metallic;

		// float NdotL = max(dot(N, L), 0.0);
		// Lo += (kD * albedo / PI + specular) * radiance * NdotL;

        float distance = length(lights[i].position - viewSpacePosition);
        vec3 lightDir = normalize(lights[i].position - viewSpacePosition);
        vec3 view = normalize(-viewSpacePosition);
        vec3 halfwayDir = normalize(lightDir + view);  

        vec3 diffuse = max(dot(N, lightDir), 0.0) * albedo * lights[i].color;
        float spec = pow(max(dot(N, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].color * spec * metallic;
        // attenuation
        // float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        float attenuation = 0.01;
        lighting += (diffuse + specular) * attenuation;
        Lo = lighting;
	}
    
    vec3 F = fresnelSchlickRoughness(max(dot(N, -V), 0.0), F0, roughness);
	vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, -V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	vec3 ambient = (kD * diffuse + specular) * ao;
	
	vec3 color = ambient + Lo + emissive; 
	color = color / (color + vec3(1.0));					// HDR tone mapping
	color = gamma ? pow(color, vec3(1.0 / 2.2)) : color;		// Gamma correction
	
	return vec4(color, 1.0f);
}

void main() {
    vec4 lighting = calcLighting();
    FragColor = lighting;
}