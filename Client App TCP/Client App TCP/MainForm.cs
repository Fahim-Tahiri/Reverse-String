using System;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Windows.Forms;

namespace ClientApp
{
    public partial class MainForm : Form
    {
        private TcpClient client;
        private StreamWriter writer;
        private StreamReader reader;

        public MainForm()
        {
            InitializeComponent();
            listView1.View = View.List;
            listView2.View = View.List;

        }

        private void ConnectToServer()
        {
            string ip = txtIP.Text;
            int port = int.Parse(txtPORT.Text);

            try
            {
                client = new TcpClient(ip, port);
                NetworkStream stream = client.GetStream();
                writer = new StreamWriter(stream, new UTF8Encoding(false)) { AutoFlush = true };
                reader = new StreamReader(stream, Encoding.UTF8);
                lblCon.ForeColor = System.Drawing.Color.Green;
              
                lblCon.Text = "Bağlandı!";
            }
            catch (Exception ex)
            {
                DisplayMessage("Hata: " + ex.Message);
            }
        }

        private void SendMessage(string message)
        {
            
                try
                {
                    writer.WriteLine(message);
                    // Read the response as a byte array
                    char[] responseBuffer = new char[1024];
                    int bytesRead = reader.Read(responseBuffer, 0, responseBuffer.Length);
                    string response = new string(responseBuffer, 0, bytesRead);
                    listView2.Items.Add("Server: " + response);

                }
                catch (Exception ex)
                {
                    DisplayMessage("Hata: " + ex.Message);
                }
            
        }

        private void btnSend_Click(object sender, EventArgs e)
        {

            if (client != null)
            {
                string message = txtInput.Text;
                if (!string.IsNullOrEmpty(message))
                {
                    listView1.Items.Add("Siz: " + message);
                    SendMessage(message);
                    txtInput.Clear();
                }
            }
        }

        private void DisplayMessage(string message)
        {
            MessageBox.Show(message);
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            writer?.Close();
            reader?.Close();
            client?.Close();
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (txtPORT.Text != "" && txtIP.Text != "")
            {
                ConnectToServer();
                btnConnect.Visible = false;
            }
            else
            {
                MessageBox.Show("Lütfen IP ve PORT bilgilerini giriniz!");
            }
        }

        private void lblCon_Click(object sender, EventArgs e)
        {
            Application.Restart();
            Environment.Exit(0);
        }

        private void txtIP_TextChanged(object sender, EventArgs e)
        {
            btnConnect.Visible = true;
        }
    }
}
