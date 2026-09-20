/* ------------------------------------------------------------
title   : usp_get_server_info
desc    : 유저 로그인, 최초생성

2026.09.16 | 김동욱 | 최초 생성
*/ -----------------------------------------------------------
CREATE PROCEDURE [dbo].[usp_get_server_info]
	@p_ip NVARCHAR(20)
AS
BEGIN
	SET NOCOUNT ON;
	
	SELECT server_no, type
	FROM [dbo].[tb_server]
	WHERE @p_ip = ip;
END
GO