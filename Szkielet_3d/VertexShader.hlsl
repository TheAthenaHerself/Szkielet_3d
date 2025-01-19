/*cbuffer vs_const_buffer_t {
    float4x4 matWorldViewProj;
    float4 padding[12];
};
struct vs_output_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
vs_output_t main(
    float3 pos : POSITION,
    float4 col : COLOR,
    row_major float4x4 mat_w : WORLD,
    uint instance_id : SV_InstanceID
) {
    vs_output_t result;
    result.position = mul(
        float4(pos, 1.0f), mul(mat_w, matWorldViewProj));
    result.color = col;
    return result;
}*/


/*cbuffer vs_const_buffer_t {
    float4x4 matWorldViewProj;
    float4x4 matWorldView;
    float4x4 matView;
    float4 colMaterial;
    float4 colLight;
    float4 dirLight;
    float4 padding;
};

struct vs_output_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

vs_output_t main(
    float3 pos : POSITION,
    float3 norm : NORMAL,
    float4 col : COLOR) {
    vs_output_t result;
    float4 NW = mul(float4(norm, 0.0f), matWorldView);
    float4 LW = mul(dirLight, matView);
    result.position = mul(float4(pos, 1.0f), matWorldViewProj);
    result.color = mul(
        max(-dot(normalize(LW), normalize(NW)), 0.0f),
        colLight * col); // lub colMaterial zamiast col
    return result;
}*/

cbuffer vs_const_buffer_t
{
    float4x4 matWorldViewProj;
    float4 padding[12]; // zapewnia buforowi sta³emu rozmiar 
 					// bêd¹cy krotnoœci¹ 256 bajtów
};

struct vs_output_t
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

vs_output_t main(float3 pos : POSITION, float4 col : COLOR)
{
    vs_output_t result;
    result.position = mul(float4(pos, 1.0f), matWorldViewProj);
    result.color = col;
    return result;
}


/*struct vs_output_t
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
vs_output_t main(float3 pos : POSITION, float4 col : COLOR)
{
    vs_output_t result;
    result.position = float4(pos.x, pos.y, pos.z, 1.0f);
    result.color = col;
    return result;
}*/
