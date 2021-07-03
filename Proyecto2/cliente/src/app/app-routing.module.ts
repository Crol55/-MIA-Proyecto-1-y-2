import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { MainComponent } from './main/main.component';
import { SignupComponent } from './signup/signup.component';
import { LoginComponent } from './login/login.component';
import { UserClienteComponent } from './user-cliente/user-cliente.component';
import { SistArchComponent } from './sist-arch/sist-arch.component';
import { UserAdminComponent } from './ADMIN/user-admin/user-admin.component';


const routes: Routes = [
   { path:'', component: MainComponent}
  ,{ path:'signup', component: SignupComponent }
  ,{ path:'login', component: LoginComponent }
  ,{ path:'main/cliente', component: UserClienteComponent }
  ,{ path:'sistemaarchivos', component: SistArchComponent }
  ,{ path:'admin/reportes' , component: UserAdminComponent }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
