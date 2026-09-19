/* ------------------------------------------------------------
title   : usp_user_login
desc    : 유저 로그인, 최초생성

2026.09.16 | 김동욱 |
*/ -----------------------------------------------------------
CREATE PROCEDURE [dbo].[usp_user_login]
	@p_user_name NVARCHAR(20)
	, @p_user_no INT OUTPUT
AS
BEGIN
	SET NOCOUNT ON;
	SET XACT_ABORT ON;
	
    MERGE INTO [dbo].[tb_user] AS target
    USING (SELECT @p_user_name AS user_name) AS source
    ON (target.user_name = source.user_name)
    WHEN NOT MATCHED THEN
        INSERT (user_name, login_time)
        VALUES (source.user_name, SYSDATETIME())
    OUTPUT inserted.user_no INTO @p_user_no;

END
GO