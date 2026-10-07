/* ------------------------------------------------------------
title   : usp_map_point_select
desc    : 맵 포인트(스폰 위치 등) 기획 정보 불러오기

2026.10.08 | 김동욱 | 최초 생성
*/ -----------------------------------------------------------
CREATE PROCEDURE [dbo].[usp_map_point_select]
AS
BEGIN
	SET NOCOUNT ON;

	SELECT point_no, map_no, type, pos_x, pos_y, pos_z
	FROM [dbo].[dt_map_point]
END
GO
