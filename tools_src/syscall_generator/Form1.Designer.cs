namespace syscall_generator
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if(disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.textBox_mustsyscallidlist = new System.Windows.Forms.TextBox();
            this.textBox_syscalllist = new System.Windows.Forms.TextBox();
            this.textBox_resultarr = new System.Windows.Forms.TextBox();
            this.textBox_resultconst = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textBox_mustsyscallidlist
            // 
            this.textBox_mustsyscallidlist.Location = new System.Drawing.Point(40, 13);
            this.textBox_mustsyscallidlist.Multiline = true;
            this.textBox_mustsyscallidlist.Name = "textBox_mustsyscallidlist";
            this.textBox_mustsyscallidlist.ReadOnly = true;
            this.textBox_mustsyscallidlist.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBox_mustsyscallidlist.Size = new System.Drawing.Size(161, 136);
            this.textBox_mustsyscallidlist.TabIndex = 0;
            this.textBox_mustsyscallidlist.Text = resources.GetString("textBox_mustsyscallidlist.Text");
            // 
            // textBox_syscalllist
            // 
            this.textBox_syscalllist.Location = new System.Drawing.Point(274, 13);
            this.textBox_syscalllist.Multiline = true;
            this.textBox_syscalllist.Name = "textBox_syscalllist";
            this.textBox_syscalllist.ReadOnly = true;
            this.textBox_syscalllist.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBox_syscalllist.Size = new System.Drawing.Size(161, 136);
            this.textBox_syscalllist.TabIndex = 1;
            this.textBox_syscalllist.Text = resources.GetString("textBox_syscalllist.Text");
            // 
            // textBox_resultarr
            // 
            this.textBox_resultarr.Location = new System.Drawing.Point(40, 196);
            this.textBox_resultarr.Multiline = true;
            this.textBox_resultarr.Name = "textBox_resultarr";
            this.textBox_resultarr.ReadOnly = true;
            this.textBox_resultarr.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBox_resultarr.Size = new System.Drawing.Size(161, 136);
            this.textBox_resultarr.TabIndex = 2;
            // 
            // textBox_resultconst
            // 
            this.textBox_resultconst.Location = new System.Drawing.Point(274, 196);
            this.textBox_resultconst.Multiline = true;
            this.textBox_resultconst.Name = "textBox_resultconst";
            this.textBox_resultconst.ReadOnly = true;
            this.textBox_resultconst.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBox_resultconst.Size = new System.Drawing.Size(161, 136);
            this.textBox_resultconst.TabIndex = 3;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(583, 169);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 4;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.textBox_resultconst);
            this.Controls.Add(this.textBox_resultarr);
            this.Controls.Add(this.textBox_syscalllist);
            this.Controls.Add(this.textBox_mustsyscallidlist);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBox_mustsyscallidlist;
        private System.Windows.Forms.TextBox textBox_syscalllist;
        private System.Windows.Forms.TextBox textBox_resultarr;
        private System.Windows.Forms.TextBox textBox_resultconst;
        private System.Windows.Forms.Button button1;
    }
}

