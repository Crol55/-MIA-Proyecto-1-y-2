import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { UserClienteComponent } from './user-cliente.component';

describe('UserClienteComponent', () => {
  let component: UserClienteComponent;
  let fixture: ComponentFixture<UserClienteComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ UserClienteComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(UserClienteComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
