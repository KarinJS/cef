param(
    [Parameter(Mandatory=$true)]
    [string]$Version,
    [string]$Channel = "spotifycdn", # 官方镜像常见：spotifycdn
    [string]$OutDir = "$(Get-Location)\third_party\cef"
)

# 示例：pwsh scripts/fetch_cef.ps1 -Version 123.1.2+gabc1234

if (!(Get-Command Expand-Archive -ErrorAction SilentlyContinue)) {
    Write-Error "需要 PowerShell 5+ 提供的 Expand-Archive"
    exit 1
}

$ErrorActionPreference = "Stop"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$base = if ($Channel -eq "spotifycdn") {
    "https://cef-builds.spotifycdn.com"
} else {
    "https://cef-builds.spotifycdn.com"
}

$name = "cef_binary_${Version}_windows64_minimal.tar.bz2"
$uri = "$base/$name"
$archive = "$OutDir\$name"

Write-Host "下载 $uri -> $archive"
Invoke-WebRequest -Uri $uri -OutFile $archive

Write-Host "解压..."
tar -xjf $archive -C $OutDir

$extracted = Get-ChildItem $OutDir | Where-Object { $_.PsIsContainer -and $_.Name -like "cef_binary_*" } | Select-Object -First 1
if ($null -eq $extracted) {
    Write-Error "未找到解压目录"
    exit 1
}

Write-Host "标准化路径 -> $OutDir"
Get-ChildItem $extracted.FullName | Move-Item -Destination $OutDir -Force
Remove-Item $extracted.FullName -Recurse -Force
Write-Host "完成，CEF_ROOT = $OutDir"
