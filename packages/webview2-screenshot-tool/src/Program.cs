using System;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using Microsoft.Web.WebView2.Core;
using Microsoft.Web.WebView2.WinForms;

internal static class Program
{
    [STAThread]
    private static void Main(string[] args)
    {
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);

        var options = Options.Parse(args);
        if (!options.IsValid(out var error))
        {
            Console.Error.WriteLine(error);
            Environment.Exit(1);
            return;
        }

        var form = new ShotForm(options);
        Application.Run(form);
    }

    private record Options(string Url, string Output, int Width, int Height, string UserDataFolder)
    {
        public static Options Parse(string[] args)
        {
            string? url = null;
            string? output = null;
            int width = 1280;
            int height = 720;
            string userData = Path.Combine(Path.GetTempPath(), "webview2-shot");

            for (int i = 0; i < args.Length; i++)
            {
                switch (args[i])
                {
                    case "--url":
                        url = args.ElementAtOrDefault(i + 1);
                        i++;
                        break;
                    case "--output":
                        output = args.ElementAtOrDefault(i + 1);
                        i++;
                        break;
                    case "--width":
                        if (int.TryParse(args.ElementAtOrDefault(i + 1), out var w))
                            width = w;
                        i++;
                        break;
                    case "--height":
                        if (int.TryParse(args.ElementAtOrDefault(i + 1), out var h))
                            height = h;
                        i++;
                        break;
                    case "--userdata":
                        var u = args.ElementAtOrDefault(i + 1);
                        if (!string.IsNullOrEmpty(u))
                            userData = u;
                        i++;
                        break;
                }
            }

            return new Options(url ?? "", output ?? "", width, height, userData);
        }

        public bool IsValid(out string error)
        {
            if (string.IsNullOrWhiteSpace(Url))
            {
                error = "Missing --url";
                return false;
            }
            if (string.IsNullOrWhiteSpace(Output))
            {
                error = "Missing --output";
                return false;
            }
            error = string.Empty;
            return true;
        }
    }

    private sealed class ShotForm : Form
    {
        private readonly Options _options;
        private readonly WebView2 _webview;

        public ShotForm(Options options)
        {
            _options = options;

            Width = options.Width;
            Height = options.Height;
            ShowInTaskbar = false;
            FormBorderStyle = FormBorderStyle.FixedToolWindow;
            StartPosition = FormStartPosition.Manual;
            Location = new System.Drawing.Point(-20000, -20000); // 隐藏窗口

            _webview = new WebView2
            {
                Dock = DockStyle.Fill
            };
            Controls.Add(_webview);

            Shown += OnShown;
        }

        private async void OnShown(object? sender, EventArgs e)
        {
            try
            {
                var env = await CoreWebView2Environment.CreateAsync(null, _options.UserDataFolder);
                await _webview.EnsureCoreWebView2Async(env);

                var tcs = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
                _webview.CoreWebView2.NavigationCompleted += (_, args) =>
                {
                    if (args.IsSuccess)
                        tcs.TrySetResult(true);
                    else
                        tcs.TrySetException(new Exception($"Navigation failed: {args.WebErrorStatus}"));
                };

                _webview.CoreWebView2.Navigate(_options.Url);
                await tcs.Task;

                Directory.CreateDirectory(Path.GetDirectoryName(_options.Output)!);
                using var fs = File.Create(_options.Output);
                await _webview.CoreWebView2.CapturePreviewAsync(CoreWebView2CapturePreviewImageFormat.Png, fs);

                Console.WriteLine($"Saved: {_options.Output}");
                Environment.ExitCode = 0;
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine(ex);
                Environment.ExitCode = 1;
            }
            finally
            {
                Close();
            }
        }
    }
}
