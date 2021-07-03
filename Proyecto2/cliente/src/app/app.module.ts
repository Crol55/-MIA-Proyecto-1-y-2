import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
//import { FormsModule } from '@angular/forms';

import { AppRoutingModule } from './app-routing.module';
import { DatosService } from './datos.service';
import { HttpClientModule } from '@angular/common/http'; // para poder utilizar httpclient desde DatosService

import { AppComponent } from './app.component';
import { SignupComponent } from './signup/signup.component';
import { MainComponent } from './main/main.component';
import { LoginComponent } from './login/login.component';
import { UserClienteComponent } from './user-cliente/user-cliente.component';
import { SistArchComponent } from './sist-arch/sist-arch.component';
import { UserAdminComponent } from './ADMIN/user-admin/user-admin.component';

@NgModule({
  declarations: [
    AppComponent,
    SignupComponent,
    MainComponent,
    LoginComponent,
    UserClienteComponent,
    SistArchComponent,
    UserAdminComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,//,FormsModule
    HttpClientModule
  ],
  providers: [DatosService], // DatosService se realiza de forma global para que todas las clases.ts las puedan utilizar sin usar 'providers'
  bootstrap: [AppComponent]
})
export class AppModule { }
