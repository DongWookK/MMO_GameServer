
param(
	[Parameter(Mandatory = $true, Position = 0)][ValidateSet('prepare', 'install')][string]$Command,
	[Parameter(Mandatory = $true, Position = 1)][string]$Map,
	[string]$RecastDemoDir = (Join-Path $PSScriptRoot '..\..\..\Tools\recastnavigation\build\RecastDemo\Release')
)

$ErrorActionPreference = 'Stop'
$inv = [System.Globalization.CultureInfo]::InvariantCulture

if (-not (Test-Path (Join-Path $RecastDemoDir 'RecastDemo.exe'))) {
	throw "RecastDemo.exe not found : $RecastDemoDir"
}

function Prepare {
	$source = Join-Path $PSScriptRoot "source\$Map.obj"
	if (-not (Test-Path $source)) { throw "source not found : $source" }

	$scale = 0.01
	$rd = @{}
	$min = @([double]::MaxValue, [double]::MaxValue, [double]::MaxValue)
	$max = @([double]::MinValue, [double]::MinValue, [double]::MinValue)
	$out = New-Object System.Text.StringBuilder

	foreach ($line in [System.IO.File]::ReadLines($source)) {
		if ($line.StartsWith('v ')) {
			$p = $line.Substring(2).Split(' ', [System.StringSplitOptions]::RemoveEmptyEntries)
			$v = @(0.0, 0.0, 0.0)
			for ($i = 0; $i -lt 3; $i++) {
				$v[$i] = [double]::Parse($p[$i], $inv) * $scale
				if ($v[$i] -lt $min[$i]) { $min[$i] = $v[$i] }
				if ($v[$i] -gt $max[$i]) { $max[$i] = $v[$i] }
			}
			[void]$out.AppendLine([string]::Format($inv, 'v {0:R} {1:R} {2:R}', $v[0], $v[1], $v[2]))
		}
		elseif ($line.StartsWith('f ')) {
			[void]$out.AppendLine($line)
		}
		elseif ($line.StartsWith('rd_')) {
			$p = $line.Split(' ', [System.StringSplitOptions]::RemoveEmptyEntries)
			if ($p.Count -eq 2) { $rd[$p[0]] = [double]::Parse($p[1], $inv) }
		}
	}

	foreach ($key in 'rd_cs', 'rd_ch', 'rd_agh', 'rd_agr', 'rd_amc', 'rd_ams', 'rd_rmis', 'rd_rmas', 'rd_mel', 'rd_mvpp', 'rd_ts') {
		if (-not $rd.ContainsKey($key)) { throw "$key not found in $source" }
	}

	$meshes = Join-Path $RecastDemoDir 'Meshes'
	[System.IO.File]::WriteAllText((Join-Path $meshes "$Map.obj"), $out.ToString())
	$cs = $rd['rd_cs'] * $scale
	$ch = $rd['rd_ch'] * $scale
	$climb = [math]::Ceiling($rd['rd_amc'] / $rd['rd_ch']) * $ch + $ch * 0.05
	$edgeMaxLen = ($rd['rd_mel'] + 0.5) * $cs
	$detailSampleDist = 600.0 / $rd['rd_cs']
	$detailSampleMaxError = 1.0 / $rd['rd_ch']
	$edgeMaxError = 1.3
	$partition = 0

	$values = @($cs, $ch, ($rd['rd_agh'] * $scale), ($rd['rd_agr'] * $scale), $climb, $rd['rd_ams'],
		$rd['rd_rmis'], $rd['rd_rmas'], $edgeMaxLen, $edgeMaxError, $rd['rd_mvpp'],
		$detailSampleDist, $detailSampleMaxError)
	$settings = ($values | ForEach-Object { ([double]$_).ToString('R', $inv) }) -join ' '
	$bounds = (($min + $max) | ForEach-Object { ([double]$_).ToString('R', $inv) }) -join ' '

	$gset = "f Meshes/$Map.obj`ns $settings $partition $bounds $([int]$rd['rd_ts'])`n"
	[System.IO.File]::WriteAllText((Join-Path $meshes "$Map.gset"), $gset)

	Write-Host "prepared : $meshes\$Map.obj, $Map.gset"
	Write-Host "  bounds(m) min($($min -join ', ')) max($($max -join ', '))"
	Write-Host "next : RecastDemo -> Sample 'Tile Mesh' -> Input Mesh '$Map.gset' -> Build -> Save"
}

function Install {
	$bin = Join-Path $RecastDemoDir 'all_tiles_navmesh.bin'
	if (-not (Test-Path $bin)) { throw "not found : $bin (RecastDemo Tile Mesh 에서 Save 를 눌렀는지 확인)" }

	foreach ($config in 'Debug', 'Release') {
		$binary = Join-Path $PSScriptRoot "..\Binary\$config"
		if (-not (Test-Path $binary)) { continue }
		$nav = Join-Path $binary 'nav'
		New-Item -ItemType Directory -Force $nav | Out-Null
		Copy-Item $bin (Join-Path $nav "$Map.navmesh") -Force
		Write-Host "installed : $nav\$Map.navmesh"
	}

	# 다른 맵을 구울 때 이전 결과를 잘못 설치하지 않도록 제거
	Remove-Item $bin
}

switch ($Command) {
	'prepare' { Prepare }
	'install' { Install }
}
