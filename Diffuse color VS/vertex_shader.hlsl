// vertex transformations world
float4x4 mWorld;
// vertex transformation view/projection
float4x4 mViewProjection;

struct VS_OUTPUT
{
	float4 Pos  : POSITION;
	float4 Color: COLOR0;
};

VS_OUTPUT main(float3 Pos  : POSITION0, float4 Color : COLOR0)
{
	VS_OUTPUT Out;
	
	// Преобразуем положение вершины в мировое пространство
	float4 pos = mul(float4(Pos, 4), mWorld);

	// Преобразуем положение вершины в пространство проекции
	Out.Pos = mul(pos, mViewProjection);

	// Меняем цвет на красный
	Out.Color = float4(1.0f, 0.0f, 0.0f, 1);
	
	return Out;
}