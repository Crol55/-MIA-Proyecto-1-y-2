
const nodemailer = require('nodemailer');

async function Enviar_Correo(correoDestino, username, nuevopwd){

    // Generate test SMTP service account from ethereal.email
    // Only needed if you don't have a real mail account for testing
   /* let testAccount = await nodemailer.createTestAccount();
  
    // create reusable transporter object using the default SMTP transport
    let transporter = nodemailer.createTransport({
      name:'example.com',
      host: "smtp.ethereal.email",
      port: 587,
      secure: false, // true for 465, false for other ports
      auth: {
        user: testAccount.user, // generated ethereal user
        pass: testAccount.pass // generated ethereal password
      },tls: {rejectUnauthorized: false}
    });*/
  
    /*var Token = crypto.randomBytes(16).toString('hex');
    var url = 'Please verify your account by clicking the link: \nhttp:\/\/localhost:5000/confirmation/' + Token ;
    let objeto =  {correo: correoDestino , 
                    token: Token
                  };
    usersSinConfirmar.push(objeto); */
  
    let transporter = nodemailer.createTransport({
        service: 'gmail', 
        auth:{
            user: 'carlosorantes77@gmail.com',
            pass: 'crol551'
        }
    });
  
    // send mail with defined transport object
    let info = await transporter.sendMail({
      from: 'carlosorantes77@gmail.com', // sender address
      to: correoDestino , // list of receivers
      subject: "CONFIRMACION ✔", // Subject line
      text: "Su cuenta ha sido creada con exito, Se le envia una contraseña temporal que sirve para"+
      " confirmar su usuario, esta contraseña temporal solo tiene vigencia de 1 minuto\n"+
      "USERNAME:" +username+ "\nCONTRASEÑA TEMPORAL:" + nuevopwd  //url//, // plain text body
      //html: "<b>Hello world?</b>" // html body
    });
  
    console.log("Message sent: %s", info.messageId);
    // Message sent: <b658f8ca-6296-ccf4-8306-87d57a0b4321@example.com>
}

 // Enviar_Correo('carlosorantes77@gmail.com', 'carlitros12', 'sapbeee');

exports.Enviar_Correo = Enviar_Correo;
