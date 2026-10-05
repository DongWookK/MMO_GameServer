/* ------------------------------------------------------------
title   : usp_pc_save_update
desc    : 캐릭터 상태 저장 (로그아웃 / 접속 종료 시)

2026.10.05 | 김동욱 | 최초 생성

return  : 0 = 성공
          1 = 저장할 캐릭터 없음 (pc_no / user_no 불일치)
*/ -----------------------------------------------------------
CREATE PROCEDURE [dbo].[usp_pc_save_update]
    @p_user_no      INT
    , @p_pc_no      BIGINT
    , @p_level      INT
    , @p_exp        BIGINT
    , @p_hp         INT
    , @p_mp         INT
    , @p_location_x FLOAT
    , @p_location_y FLOAT
    , @p_location_z FLOAT
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;

    DECLARE @a_error INT = 0;

    UPDATE [dbo].[tb_pc]
    SET level = @p_level
        , exp = @p_exp
        , hp = @p_hp
        , mp = @p_mp
        , location_x = @p_location_x
        , location_y = @p_location_y
        , location_z = @p_location_z
    WHERE pc_no = @p_pc_no
        AND user_no = @p_user_no;

    IF @@ROWCOUNT = 0
    BEGIN
        SET @a_error = 1;
    END

    RETURN @a_error;
END
GO
