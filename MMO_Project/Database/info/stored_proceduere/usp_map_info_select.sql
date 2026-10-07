/* ------------------------------------------------------------
title   : usp_map_info_select
desc    : 맵 기획 정보 불러오기

2026.10.07 | 김동욱 | 최초 생성
*/ -----------------------------------------------------------
CREATE PROCEDURE [dbo].[usp_map_info_select]
AS
BEGIN
	SET NOCOUNT ON;
	
	SELECT map_no, type, map_name, filename
	FROM [dbo].[dt_map]
END
GO