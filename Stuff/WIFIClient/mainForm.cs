using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.Net.Sockets;

namespace UA3REO_WIFIClient
{
    public partial class mainForm : Form
    {
        public static SerialPort port = new SerialPort();
        private Thread mainThread = new Thread(new ThreadStart(thread_loop));
        public static TcpClient tcp_client = new TcpClient();
        public static System.Windows.Forms.Label statusLabelStatic;
        public static string address = "";

        public mainForm()
        {
            InitializeComponent();
            statusLabelStatic = statusLabel;
        }

        private void mainForm_Load(object sender, EventArgs e)
        {
            scanPorts();
            addressTextBox.Text = Properties.Settings.Default.address;
            portComboBox.Text = Properties.Settings.Default.com_port;
        }
        private void scanPorts()
        {
            portComboBox.Items.Clear();
            foreach (string lport in SerialPort.GetPortNames())
                portComboBox.Items.Add(lport);
        }

        private void portComboBox_DropDown(object sender, EventArgs e)
        {
            scanPorts();
        }

        private void mainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            mainThread.Abort();
            Properties.Settings.Default.address = addressTextBox.Text;
            Properties.Settings.Default.com_port = portComboBox.Text;
            Properties.Settings.Default.Save();
            if (port.IsOpen)
                port.Close();
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            if (!port.IsOpen)
            {
                port = new SerialPort(portComboBox.SelectedItem.ToString(), 38400, Parity.None, 8);
                port.ReadTimeout = 5000;
                port.Encoding = Encoding.ASCII;
                port.ReadBufferSize = 65000;
                try
                {
                    port.Open();
                    port.ReadExisting();
                }
                catch
                {
                    statusLabel.Text = "Ошибка открытия порта";
                }
            }
            if (tcp_client.Connected) tcp_client.Close();
            try
            {
                tcp_client.ReceiveTimeout = 5000;
                tcp_client.SendTimeout = 5000;
                tcp_client.NoDelay = true;
            }
            catch { }
            connect();
            address = addressTextBox.Text;
            try { mainThread.Start(); } catch { }
        }

        public static void setStatus(string text)
        {
            try
            {
                statusLabelStatic.Invoke((MethodInvoker)delegate
                {
                    // Running on the UI thread
                    Console.WriteLine(text);
                    statusLabelStatic.Text = text;
                });
            }
            catch { }
        }

        public static void connect()
        {
            try
            {
                setStatus("Соединяюсь с трансивером");
                tcp_client = new TcpClient();
                tcp_client.Connect(address, 6784);
                setStatus("Соединение установлено, жду команды");
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                setStatus(e.Message);
            }
        }

        public static void thread_loop()
        {
            ASCIIEncoding ascii = new ASCIIEncoding();
            string buffer = "";
            while (true)
            {
                if (port.BytesToRead > 0 || buffer.IndexOf(";") > 0)
                {
                    buffer += port.ReadExisting();
                    if (buffer.IndexOf(";") > 0)
                    {
                        string command = buffer.Substring(0, buffer.IndexOf(";"));
                        buffer = buffer.Substring(buffer.IndexOf(";") + 1);

                        command = command.Trim()+"\n";
                        if (command != "")
                        {
                            setStatus("Получена команда: " + command.Trim());
                            byte[] send_buffer = ascii.GetBytes(command);
                            try
                            {
                                Console.WriteLine("Отправляю на сервер: " + ascii.GetString(send_buffer).Trim());
                                tcp_client.GetStream().Write(send_buffer, 0, command.Length);
                            }
                            catch
                            { connect(); }
                        }
                    }
                }
                if(tcp_client.Connected && tcp_client.GetStream().DataAvailable)
                {
                    int len = tcp_client.Available;
                    byte[] read_buffer = new byte[len];
                    tcp_client.GetStream().Read(read_buffer, 0, len);
                    string answer = ascii.GetString(read_buffer);
                    setStatus("Получен ответ: " + answer);
                    port.Write(answer);
                }
                Thread.Sleep(1);
            }
        }
    }
}
